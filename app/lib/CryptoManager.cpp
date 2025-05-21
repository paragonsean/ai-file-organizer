#include "CryptoManager.hpp"
#include "Utils.hpp"
#include <cstring>
#include <openssl/types.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <iostream>


/**
 * Constructs a CryptoManager instance with the provided environment variables.
 *
 * This constructor initializes the CryptoManager object with two parts of an obfuscated
 * key and an encrypted data string, which are retrieved from environment variables.
 * These are used in the reconstruction of the full API key.
 *
 * @param env_pc The environment variable containing the first part of the obfuscated key.
 * @param env_rr The environment variable containing the encrypted data string.
 */

CryptoManager::CryptoManager(const std::string& env_pc, const std::string& env_rr)
    : env_pc(env_pc), env_rr(env_rr) {}

/**
 * Reconstructs the full API key from the environment variables.
 *
 * This function takes the two parts of the API key that are stored in the environment
 * variables, deobfuscates and reassembles them, and then decrypts the encrypted
 * data stored in the environment variable using the reassembled key.
 *
 * @return A string containing the full API key.
 */
std::string CryptoManager::reconstruct()
{
    std::string kp_1 = deobfuscate(env_pc);
    std::string kp_2 = deobfuscate(std::string(CryptoManager::embedded_pc));
    std::string recomposed_key = reassemble_key(kp_1, kp_2);
    std::string k = aes256_decrypt(Utils::hex_to_vector(env_rr), recomposed_key);

    return k;
}

/**
 * Reassembles a key from two parts.
 *
 * This function takes two parts of a key and reassembles them into a single key.
 * The key is reassembled by concatenating the two parts together, with the second
 * part coming first. This is the reverse of how the key was split, because the
 * second part is the one that was originally first.
 *
 * @param part1 The first part of the key.
 * @param part2 The second part of the key.
 * @return A string containing the reassembled key.
 */
std::string CryptoManager::reassemble_key(const std::string& part1, const std::string& part2)
{
    return part2 + part1;
}


/**
 * Decodes a Base64-encoded string.
 *
 * This function takes a Base64-encoded string and decodes it into its original
 * form. It uses OpenSSL's BIO functions to perform the decoding. If the decoding
 * process fails, a std::runtime_error is thrown.
 *
 * @param encoded The Base64-encoded string to be decoded.
 * @return A string containing the decoded data.
 * @throws std::runtime_error if the Base64 decoding fails.
 */

std::string CryptoManager::base64_decode(const std::string& encoded) {
    BIO *bio, *b64;
    int decode_len = encoded.size() * 3 / 4; // Approximation of the decoded size
    char *buffer = new char[decode_len + 1]; // +1 for the null character
    memset(buffer, 0, decode_len + 1);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Ignore newline characters
    bio = BIO_new_mem_buf(encoded.c_str(), -1); // Buffer creation
    bio = BIO_push(b64, bio);

    int decoded_length = BIO_read(bio, buffer, decode_len);
    if (decoded_length < 0) {
        delete[] buffer;
        BIO_free_all(bio);
        throw std::runtime_error("Erreur dans le décodage Base64.");
    }

    BIO_free_all(bio);

    std::string decoded(buffer, decoded_length);
    delete[] buffer;
    return decoded;
}


/**
 * Deobfuscates the given obfuscated data using the provided salt by applying
 * an XOR operation between each character of the obfuscated data and the 
 * corresponding character in the salt. The salt is repeated cyclically if 
 * it is shorter than the obfuscated data.
 *
 * @param obfuscated_data A string containing the obfuscated data to be deobfuscated.
 * @param salt A string used to XOR with the obfuscated data to retrieve the original data.
 *
 * @return A string containing the deobfuscated original data.
 */

std::string CryptoManager::deobfuscate_with_salt(const std::string& obfuscated_data,
                                                 const std::string& salt)
{
    std::string decoded_data;
    size_t salt_index = 0;

    for (char c : obfuscated_data) {
        char original_char = c ^ salt[salt_index % salt.length()];
        salt_index++;

        decoded_data += original_char;
    }

    return decoded_data;
}


/**
 * Deobfuscates the given data by first extracting the salt and the Base64-encoded
 * obfuscated data, then decoding the Base64-encoded data and applying the XOR
 * operation with the salt to obtain the original data.
 *
 * @param obfuscated_data A string containing the obfuscated data, which must be
 *                        at least 16 bytes longer than the salt length.
 *
 * @return A string containing the deobfuscated data.
 *
 * @throws std::runtime_error if the obfuscated_data is too short
 */
std::string CryptoManager::deobfuscate(const std::string& obfuscated_data) {
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

std::string CryptoManager::aes256_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key) {
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
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.data(), iv) != 1) {
            throw std::runtime_error("Decryption initialization failed.");
        }

        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data() + sizeof(iv), ciphertext.size() - sizeof(iv)) != 1) {
            throw std::runtime_error("Decryption failed.");
        }
        plaintext_len += len;

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            throw std::runtime_error("Final decryption step failed.");
        }
        plaintext_len += len;
        plaintext.resize(plaintext_len);
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    EVP_CIPHER_CTX_free(ctx);
    return std::string(plaintext.begin(), plaintext.end());
}