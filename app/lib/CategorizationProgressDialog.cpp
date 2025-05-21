#include "CategorizationProgressDialog.hpp"
#include "MainApp.hpp"
#include <gtk/gtk.h>
#include <gtk/gtktypes.h>
#include <gobject/gsignal.h>


/**
 * @brief CategorizationProgressDialog constructor.
 * @param parent A pointer to the parent window that this dialog should be transient for.
 * @param main_app A pointer to the MainApp object that owns this dialog.
 * @param show_subcategory_col TRUE if the subcategory column should be shown in the dialog, FALSE otherwise.
 *
 * This constructor creates a new progress dialog for categorization. The dialog is created by calling the
 * create_categorization_progress_dialog() function, which is a member of this class. The resulting dialog is
 * stored in the m_Dialog member variable. The text view and stop button widgets are also retrieved from the
 * dialog and stored in the m_TextView and m_StopButton member variables, respectively. Finally, the buffer
 * associated with the text view is retrieved and stored in the buffer member variable.
 *
 * The stop button is connected to a signal handler that will be called when the button is clicked. The signal
 * handler will call the append_text() function on the MainApp object, passing a string that indicates that the
 * stop button was clicked. The signal handler will also set the stop_analysis flag on the MainApp object to
 * TRUE, which will cause the categorization thread to exit.
 */
CategorizationProgressDialog::CategorizationProgressDialog(GtkWindow* parent, MainApp *main_app, gboolean show_subcategory_col)
    : m_MainApp(main_app), m_Dialog(nullptr), m_TextView(nullptr), m_StopButton(nullptr), buffer(nullptr)
{
    // Create the dialog
    m_Dialog = create_categorization_progress_dialog(parent);

    if (m_Dialog) {
        m_TextView = GTK_WIDGET(g_object_get_data(G_OBJECT(m_Dialog), "progress_text_view"));
        m_StopButton = GTK_WIDGET(g_object_get_data(G_OBJECT(m_Dialog), "stop_analysis_button"));

        if (m_TextView) {
            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_TextView));
        }

        if (!m_StopButton) {
            g_warning("Stop button not found in the dialog!");
            return;
        }

        g_signal_connect(
            m_StopButton,
            "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer user_data) {
                MainApp* app = static_cast<MainApp*>(user_data);
                if (!app) {
                    g_warning("MainApp pointer is null!");
                    return;
                }
                std::string message = "\nStop button clicked.\n";
                app->progress_dialog->append_text(message);
                app->stop_analysis = true;
            }),
            m_MainApp
        );
    }
}


/**
 * @brief Creates a new dialog for categorization progress.
 * @param parent The parent window that this dialog should be transient for.
 * @return A pointer to the newly created dialog.
 *
 * This function creates a new dialog with a text view and a stop button. The text view is
 * configured to wrap words and to not be editable. The stop button is connected to a signal
 * handler that will be called when the button is clicked. The dialog is set to be modal and
 * transient for the parent window, and is given a title of "Analyzing Files". The dialog
 * is also given a default size of 800x1000 pixels. The text view and stop button are both
 * stored in the m_TextView and m_StopButton member variables, respectively, so that they
 * can be accessed later. Finally, the dialog is shown and the function returns a pointer to
 * the newly created dialog.
 */
GtkWidget* CategorizationProgressDialog::create_categorization_progress_dialog(GtkWindow *parent)
{
    GtkWidget *dialog, *content_area, *scrolled_window, *button_box, *stop_button;

    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Analyzing Files");
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    if (!dialog) {
        g_critical("Failed to create dialog.");
        return nullptr;
    }

    gtk_window_set_default_size(GTK_WINDOW(dialog), 800, 1000);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(content_area), scrolled_window, TRUE, TRUE, 10);
    m_TextView = gtk_text_view_new();
    g_object_ref(m_TextView);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_TextView));

    // Configure the text view
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(m_TextView), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_TextView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(m_TextView), FALSE);

    gtk_container_add(GTK_CONTAINER(scrolled_window), m_TextView);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);

    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_END);
    gtk_box_pack_start(GTK_BOX(content_area), button_box, FALSE, TRUE, 10);

    stop_button = gtk_button_new_with_label("Stop Analysis");
    gtk_widget_set_margin_end(stop_button, 10);
    gtk_container_add(GTK_CONTAINER(button_box), stop_button);

    gtk_widget_show_all(dialog);

    g_object_set_data(G_OBJECT(dialog), "progress_text_view", m_TextView);
    g_object_set_data(G_OBJECT(dialog), "stop_analysis_button", stop_button);

    return dialog;
}


/**
 * @brief Shows the categorization progress dialog.
 *
 * This function shows the categorization progress dialog, which was created by
 * calling the create_categorization_progress_dialog() function. If the dialog
 * does not exist, this function does nothing.
 */
void CategorizationProgressDialog::show()
{
    if (m_Dialog) {
        gtk_widget_show(m_Dialog);
    }
}


/**
 * @brief Appends text to the text view in the categorization progress dialog.
 * 
 * This function appends the provided text to the text buffer associated with
 * the text view widget in the dialog. It checks if the text view is initialized
 * before attempting to insert the text. If the text view is not initialized, 
 * an error message is printed and the function returns without performing any action.
 * 
 * @param text The string to be appended to the text view.
 */

void CategorizationProgressDialog::append_text(const std::string& text)
{
    if (!m_TextView) {
        g_printerr("Error: text_view is not initialized!\n");
        return;
    }

    gtk_text_buffer_insert_at_cursor(buffer, text.c_str(), -1);
}


/**
 * @brief Hides the categorization progress dialog.
 *
 * If the dialog is valid, this function will hide it. Otherwise, it does nothing.
 */
void CategorizationProgressDialog::hide()
{
    if (m_Dialog) {
        gtk_widget_hide(m_Dialog);
    }
}


/**
 * @brief Destroys the categorization progress dialog and its associated widgets.
 *
 * This function unreferences the text view widget and destroys the dialog, if
 * they have been initialized. It also sets the stop button and text buffer
 * member variables to nullptr.
 */
CategorizationProgressDialog::~CategorizationProgressDialog()
{
    if (m_Dialog) {
        gtk_widget_destroy(m_Dialog);
        m_Dialog = nullptr;
    }

    if (m_TextView) {
        g_object_unref(m_TextView);
        m_TextView = nullptr;
    }

    m_StopButton = nullptr;
    buffer = nullptr;
}