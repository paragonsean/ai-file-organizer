#include "MainAppEditActions.hpp"


/**
 * Pastes the text in the clipboard into the GtkEditable path_entry at the current position.
 *
 * @param path_entry The GtkEntry to paste the text into.
 */
void MainAppEditActions::on_paste(GtkEntry* path_entry) {
    GtkEditable* editable = GTK_EDITABLE(path_entry);
    gchar* clipboard_text = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    if (clipboard_text) {
        gint cursor_pos = gtk_editable_get_position(editable);
        gtk_editable_insert_text(editable, clipboard_text, -1, &cursor_pos);
        g_free(clipboard_text);
    }
}


/**
 * Copies the selected text in the path_entry to the clipboard.
 *
 * @param path_entry the widget to copy from
 */
void MainAppEditActions::on_copy(GtkEntry* path_entry) {
    GtkEditable* editable = GTK_EDITABLE(path_entry);
    gchar* selected_text = get_and_delete_selection(editable, FALSE);

    if (selected_text) {
        copy_to_clipboard(selected_text);
        g_free(selected_text);
    }
}


/**
 * Cuts the selected text in the path_entry to the clipboard and removes it.
 *
 * @param path_entry the widget to cut from
 */
void MainAppEditActions::on_cut(GtkEntry* path_entry) {
    GtkEditable* editable = GTK_EDITABLE(path_entry);
    gchar* selected_text = get_and_delete_selection(editable, TRUE); // Explicitly pass TRUE

    if (selected_text) {
        copy_to_clipboard(selected_text);
        g_free(selected_text);
    }
}


/**
 * Deletes the currently selected text in the path_entry.
 *
 * @param path_entry the widget to delete the selection from
 */
void MainAppEditActions::on_delete(GtkEntry* path_entry) {
    GtkEditable* editable = GTK_EDITABLE(path_entry);
    get_and_delete_selection(editable, TRUE); // Explicitly pass TRUE to delete the selection
}


/**
 * Copies the given text to the clipboard.
 *
 * @param text the text to copy; if nullptr, does nothing
 */
void MainAppEditActions::copy_to_clipboard(const gchar* text)
{
    if (text) {
        gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), text, -1);
    }
}


/**
 * Retrieves the currently selected text from the given editable widget,
 * and if requested (default), deletes the selection from the widget.
 *
 * @param editable the widget to get the selection from
 * @param delete_selection if TRUE, deletes the selection from the widget;
 *                         if FALSE, just returns the selected text
 * @return the selected text, or nullptr if there is no selection;
 *         the caller is responsible for freeing this string
 */
gchar* MainAppEditActions::get_and_delete_selection(GtkEditable* editable, gboolean delete_selection = TRUE)
{
    gint start, end;

    if (gtk_editable_get_selection_bounds(editable, &start, &end)) {
        gchar* selected_text = gtk_editable_get_chars(editable, start, end);

        if (delete_selection) {
            gtk_editable_delete_text(editable, start, end);
        }

        return selected_text; // Caller must free this
    }

    return nullptr; // No selection
}