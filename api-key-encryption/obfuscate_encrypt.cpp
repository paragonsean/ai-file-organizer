#include <utility>
#include <stddef.h>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <cstdlib>

// Need setenv() before including dotenv.h on Windows
int setenv(const char *name, const char *value, int overwrite) {
    if (!overwrite && getenv(name)) return 0;
    return _putenv_s(name, value);
}
#endif

#include "../app/include/external/dotenv.h"
#include <stdexcept>
#include <iostream>
#include <random>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>


/**
 * Encodes the input string into Base64 format.
 *
 * This function takes a standard string input and converts it into a 
 * Base64-encoded string. It uses OpenSSL's BIO library functions to 
 * perform the encoding. The resulting Base64 string does not contain 
 * newline characters.
 *
 * @param input The string to be encoded.
 * @return A Base64-encoded string.
 */

std::string base64_encode(const std::string& input) {
    BIO *bio, *b64;
    BUF_MEM *buffer_ptr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), input.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buffer_ptr);

    std::string encoded(buffer_ptr->data, buffer_ptr->length);
    BIO_free_all(bio);

    return encoded;
}


/**
 * Decodes a Base64-encoded string.
 *
 * This function takes a Base64-encoded string and decodes it into its original
 * form. It uses OpenSSL's BIO library functions to perform the decoding. If
 * the decoding process fails, a std::runtime_error is thrown.
 *
 * @param encoded The Base64-encoded string to be decoded.
 * @return A string containing the decoded data.
 * @throws std::runtime_error if the Base64 decoding fails.
 */
std::string base64_decode(const std::string& encoded) {
    BIO *bio, *b64;
    int decode_len = encoded.size() * 3 / 4; // Approximation of the decoded size
    char *buffer = new char[decode_len + 1]; // +1 for the null character
    memset(buffer, 0, decode_len + 1);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines
    bio = BIO_new_mem_buf(encoded.c_str(), -1); // Creation of buffer
    bio = BIO_push(b64, bio);

    int decoded_length = BIO_read(bio, buffer, decode_len);
    if (decoded_length < 0) {
        delete[] buffer;
        BIO_free_all(bio);
        throw std::runtime_error("Could not decode Base64.");
    }

    BIO_free_all(bio);

    std::string decoded(buffer, decoded_length);
    delete[] buffer;
    return decoded;
}


/**
 * Splits an API key into two parts.
 *
 * This function takes an API key and splits it into two equal-sized parts. The
 * key must be at least two characters long to split. If the key is too short,
 * a std::invalid_argument exception is thrown.
 *
 * @param api_key The API key to be split.
 * @return A pair of strings, each containing one half of the API key.
 * @throws std::invalid_argument if the API key is too short to split.
 */
std::pair<std::string, std::string> decompose_key(const std::string& api_key)
{
    // Ensure the key is long enough to split
    if (api_key.length() < 2) {
        throw std::invalid_argument("API key is too short to split.");
    }

    // Split the key into two halves
    size_t mid = api_key.length() / 2;
    std::string part1 = api_key.substr(0, mid);
    std::string part2 = api_key.substr(mid);

    return std::make_pair(part1, part2);
}


/**
 * Reassembles a key by concatenating two parts.
 *
 * This function takes two parts of a key and combines them into a single string.
 * The parts are concatenated in the order provided.
 *
 * @param part1 The first part of the key.
 * @param part2 The second part of the key.
 * @return A string containing the reassembled key.
 */

std::string reassemble_key(const std::string& part1, const std::string& part2)
{
    return part1 + part2;
}


/**
 * Generates a random salt of the given length.
 *
 * The salt is a string of the given length composed of characters from the
 * set of lowercase letters, uppercase letters, and digits. The default length
 * is 16.
 *
 * @param length The length of the salt to generate. Defaults to 16.
 *
 * @return A randomly generated salt of the given length.
 */
std::string generate_random_salt(size_t length = 16) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t max_index = sizeof(charset) - 2;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, max_index);

    std::string salt;
    salt.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        salt += charset[distribution(generator)];
    }
    return salt;
}


/**
 * Obfuscates the given data using the provided salt by applying
 * an XOR operation between each character of the data and the 
 * corresponding character in the salt. The salt is repeated 
 * cyclically if it is shorter than the data.
 *
 * @param data A string containing the data to be obfuscated.
 * @param salt A string used to XOR with the data to generate the obfuscated data.
 *
 * @return A string containing the obfuscated data.
 */

std::string obfuscate_with_salt(const std::string& data, const std::string& salt) {
    std::string obfuscated_data;
    size_t salt_index = 0;
    
    for (char c : data) {
        obfuscated_data += c ^ salt[salt_index % salt.length()];
        salt_index++;
    }

    return obfuscated_data;
}


/**
 * Deobfuscates the given data using the provided salt by applying
 * an XOR operation between each character of the obfuscated data and the
 * corresponding character in the salt. The salt is repeated cyclically if
 * it is shorter than the obfuscated data.
 *
 * @param obfuscated_data A string containing the obfuscated data to be deobfuscated.
 * @param salt A string used to XOR with the obfuscated data to retrieve the original data.
 *
 * @return A string containing the deobfuscated original data.
 */
std::string deobfuscate_with_salt(const std::string& obfuscated_data, const std::string& salt) {
    std::string decoded_data;
    size_t salt_index = 0;

    // Iterate over each character in the obfuscated data
    for (char c : obfuscated_data) {
        // XOR with the corresponding salt character
        char original_char = c ^ salt[salt_index % salt.length()];
        salt_index++;

        decoded_data += original_char;
    }

    return decoded_data;
}


/**
 * Deobfuscates a given string by reversing the obfuscation process.
 *
 * This function assumes that the input string is composed of a salt 
 * prefixed to a Base64-encoded obfuscated data. The salt is used in 
 * conjunction with an XOR operation to deobfuscate the data. The 
 * function extracts the salt from the input, decodes the remaining 
 * Base64-encoded string, and then applies the inverse of the 
 * obfuscation algorithm to retrieve the original data.
 *
 * @param obfuscated_data The obfuscated string that includes both the 
 *                        salt and the Base64-encoded data.
 * @return The original deobfuscated string.
 * @throws std::runtime_error if the input string is too short to 
 *                            contain both a salt and obfuscated data.
 */

std::string deobfuscate(const std::string& obfuscated_data) {
    constexpr size_t salt_length = 16;

    if (obfuscated_data.length() <= salt_length) {
        throw std::runtime_error("Invalid data: Salt or obfuscated data missing.");
    }

    std::string salt = obfuscated_data.substr(0, salt_length);
    std::string base64_encoded = obfuscated_data.substr(salt_length);
    std::string obfuscated_data_with_salt = base64_decode(base64_encoded);

    return deobfuscate_with_salt(obfuscated_data_with_salt, salt);
}


/**
 * Obfuscates a given string with a given salt, and encodes it with Base64.
 *
 * This function first obfuscates the input string by applying an XOR operation
 * with the given salt. The result is then encoded with Base64 and prefixed with
 * the salt. The output is a string that includes both the salt and the obfuscated
 * data.
 *
 * @param data The string to be obfuscated.
 * @param salt The salt used for obfuscation.
 * @return The obfuscated string, including the salt and the Base64-encoded
 *         obfuscated data.
 */
std::string obfuscate(const std::string& data, std::string& salt) {
    std::cout << "Salt: " << salt << std::endl;

    // Obfuscate data with the salt
    std::string obfuscated_data = obfuscate_with_salt(data, salt);

    // Encode obfuscated data with Base64
    std::string base64_encoded = base64_encode(obfuscated_data);

    return salt + base64_encoded;
}


/**
 * Encrypts a given string with a given key using AES-256-CBC.
 *
 * This function takes a plaintext string and a key as input, and returns the
 * encrypted data as a vector of unsigned characters. The key must be 32 bytes
 * (256 bits) long. The function first generates a random initialization vector
 * (IV) and prepends it to the ciphertext. The output is a vector of unsigned
 * characters that includes both the IV and the encrypted data.
 *
 * @param plaintext The string to be encrypted.
 * @param key The 32-byte (256-bit) key used for encryption.
 * @return The encrypted data as a vector of unsigned characters.
 * @throws std::runtime_error if the key length is not 32 bytes or if there is
 *                            an error during encryption.
 */
std::vector<unsigned char> aes256_encrypt(const std::string& plaintext, const std::string& key) {
    // Ensure the key length is 32 bytes (256 bits)
    if (key.size() != 32) {
        throw std::runtime_error("Key must be 32 bytes (256 bits) for AES-256 encryption.");
    }

    // Initialization vector (16 bytes for AES-CBC)
    unsigned char iv[16];
    if (!RAND_bytes(iv, sizeof(iv))) {
        throw std::runtime_error("Failed to generate random IV.");
    }

    // Output buffer
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, ciphertext_len = 0;

    // Create and initialize the context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX.");
    }

    try {
        // Initialize encryption operation
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.data(), iv) != 1) {
            throw std::runtime_error("Encryption initialization failed.");
        }

        // Encrypt the plaintext
        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, (unsigned char*)plaintext.data(), plaintext.size()) != 1) {
            throw std::runtime_error("Encryption failed.");
        }
        ciphertext_len += len;

        // Finalize encryption
        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            throw std::runtime_error("Final encryption step failed.");
        }
        ciphertext_len += len;

        // Add the IV to the start of the ciphertext
        ciphertext.insert(ciphertext.begin(), iv, iv + sizeof(iv));
        ciphertext.resize(ciphertext_len + sizeof(iv)); // Resize to actual ciphertext length
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}


/**
 * Decrypts the given ciphertext using AES-256 in CBC mode.
 *
 * This function takes a ciphertext and a key, both provided as inputs, and
 * decrypts the ciphertext using the AES-256 algorithm with CBC mode of operation.
 * The key must be 32 bytes long. The ciphertext must contain a 16-byte
 * initialization vector (IV) at its beginning, which is used during the decryption
 * process.
 *
 * @param ciphertext A vector of unsigned char containing the encrypted data,
 *                   with the first 16 bytes being the IV.
 * @param key A string representing the decryption key, which must be 32 bytes long.
 *
 * @return A string containing the decrypted plaintext.
 *
 * @throws std::runtime_error if the key is not 32 bytes, if the ciphertext is too short
 *                            to contain an IV, if the decryption context cannot be created,
 *                            or if any step of the decryption fails.
 */
std::string aes256_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    if (key.size() != 32) {
        throw std::runtime_error("Key must be 32 bytes (256 bits) for AES-256 decryption.");
    }

    // Extract the IV (first 16 bytes of the ciphertext)
    if (ciphertext.size() < 16) {
        throw std::runtime_error("Ciphertext is too short to contain an IV.");
    }
    unsigned char iv[16];
    std::memcpy(iv, ciphertext.data(), sizeof(iv));

    // Output buffer
    std::vector<unsigned char> plaintext(ciphertext.size());
    int len = 0, plaintext_len = 0;

    // Create and initialize the context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX.");
    }

    try {
        // Initialize decryption operation
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.data(), iv) != 1) {
            throw std::runtime_error("Decryption initialization failed.");
        }

        // Decrypt the ciphertext
        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data() + sizeof(iv), ciphertext.size() - sizeof(iv)) != 1) {
            throw std::runtime_error("Decryption failed.");
        }
        plaintext_len += len;

        // Finalize decryption
        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            throw std::runtime_error("Final decryption step failed.");
        }
        plaintext_len += len;
        plaintext.resize(plaintext_len); // Resize to actual plaintext length
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    EVP_CIPHER_CTX_free(ctx);
    return std::string(plaintext.begin(), plaintext.end());
}


/**
 * The main function of the program.
 *
 * It reads the API key and the secret key from the environment variables,
 * decomposes the secret key into two parts, obfuscates each part, prints out
 * the obfuscated parts, deobfuscates the parts, reassembles the key, checks
 * if the recomposed key matches the original secret key, encrypts the API key
 * using the secret key, prints out the encrypted data in hex, decrypts the
 * data, and prints out the decrypted data.
 */
int main()
{
    dotenv::init("encryption.ini");

    std::string api_key = std::getenv("LLM_API_KEY");
    std::string secret_key = std::getenv("SECRET_KEY");

    std::pair<std::string, std::string> decomposed_key = decompose_key(secret_key);
    
    std::string salt1 = generate_random_salt();
    std::string salt2 = generate_random_salt();

    std::string obfuscated_key_part_1 = obfuscate(decomposed_key.first, salt1);
    std::string obfuscated_key_part_2 = obfuscate(decomposed_key.second, salt2);

    std::cout << "Obfuscated Key part 1: " << obfuscated_key_part_1 << std::endl;
    std::cout << "Obfuscated Key part 2: " << obfuscated_key_part_2 << std::endl;

    std::string deobfuscated_key_part_1 = deobfuscate(obfuscated_key_part_1);
    std::string deobfuscated_key_part_2 = deobfuscate(obfuscated_key_part_2);

    std::cout << "Deobfuscated Key part 1: " << deobfuscated_key_part_1 << std::endl;
    std::cout << "Deobfuscated Key part 2: " << deobfuscated_key_part_2 << std::endl;

    std::string recomposed_key = reassemble_key(deobfuscated_key_part_1, deobfuscated_key_part_2);

    if (secret_key == recomposed_key) {
        std::cout << "Recomposed key matches the secret key!" << std::endl;
    } else {
        std::cout << "Recomposed key does NOT match the secret key!!" << std::endl;
    }

    std::vector<unsigned char> encrypted_data = aes256_encrypt(api_key, secret_key);
    std::cout << "Encrypted data (hex): ";
    for (unsigned char c : encrypted_data) {
        printf("%02x", c);
    }
    std::cout << std::endl;

    std::string decrypted_data = aes256_decrypt(encrypted_data, secret_key);
    std::cout << "Decrypted data: " << decrypted_data << std::endl;
}