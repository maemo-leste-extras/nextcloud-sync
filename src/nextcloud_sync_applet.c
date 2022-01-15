#include <gconf/gconf-client.h>
#include <gtk/gtk.h>

#include <hildon/hildon.h>
#include <hildon-cp-plugin/hildon-cp-plugin-interface.h>

/* TODO:
 * Proxy support
 * Exclusions
 * Custom DAV path
 * bandwidth limits
 * Allow multiple profiles
 * Trigger for manual sync
 */
#define GCONF_KEY_NEXTCLOUD_PATH "/system/maemo/sync/nextcloud/sync-path"
#define GCONF_KEY_NEXTCLOUD_HOST "/system/maemo/sync/nextcloud/host"
#define GCONF_KEY_NEXTCLOUD_USER "/system/maemo/sync/nextcloud/username"
#define GCONF_KEY_NEXTCLOUD_PASS "/system/maemo/sync/nextcloud/password"
#define GCONF_KEY_NEXTCLOUD_AUTO "/system/maemo/sync/nextcloud/autosync"

typedef struct {
	osso_context_t *osso;
	HildonWindow *parent;
	gboolean state;
	GtkWidget *dialog;
	GtkWidget *pan;
	GtkWidget *box;
	GtkWidget *vbox;
	GtkWidget *host_hbox;
	GtkWidget *user_hbox;
	GtkWidget *pass_hbox;
	GtkWidget *path_label;
	GtkWidget *path_entry;
	GtkWidget *autosync_check_btn;
	GtkWidget *host_label;
	GtkWidget *host_entry;
	GtkWidget *user_label;
	GtkWidget *user_entry;
	GtkWidget *pass_label;
	GtkWidget *pass_entry;
} cpa_dialog;

static gboolean cpa_save_settings(cpa_dialog * dialog)
{
	// TODO: Check path is writable
	(void)dialog;
	return TRUE;
}

static void cpa_hide_ui(cpa_dialog * dialog)
{
	gtk_widget_hide(dialog->pass_entry);
	gtk_widget_hide(dialog->pass_label);
	gtk_widget_hide(dialog->user_entry);
	gtk_widget_hide(dialog->user_label);
	gtk_widget_hide(dialog->host_entry);
	gtk_widget_hide(dialog->host_label);
	gtk_widget_hide(dialog->autosync_check_btn);
	gtk_widget_hide(dialog->path_entry);
	gtk_widget_hide(dialog->path_label);
	gtk_widget_hide(dialog->host_hbox);
	gtk_widget_hide(dialog->user_hbox);
	gtk_widget_hide(dialog->pass_hbox);
	gtk_widget_hide(dialog->vbox);
	gtk_widget_hide(dialog->box);
	gtk_widget_hide(dialog->pan);
	gtk_widget_hide(dialog->dialog);
	gtk_widget_queue_resize(dialog->dialog);
}

static gboolean cpa_validate_settings(cpa_dialog * dialog)
{
	const gchar *path = gtk_entry_get_text(GTK_ENTRY(dialog->path_entry));
	/* const gchar *host = gtk_entry_get_text(GTK_ENTRY(dialog->host_entry)); */
	const gchar *user = gtk_entry_get_text(GTK_ENTRY(dialog->user_entry));
	const gchar *pass = gtk_entry_get_text(GTK_ENTRY(dialog->pass_entry));

	if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
		hildon_banner_show_information(NULL, NULL,
					       "Chosen path is a regular file. Please change it.");
		return TRUE;
	}

	/* TODO: Create directory */

	/*
	   if (!g_uri_is_valid(host, G_URI_FLAGS_NONE, NULL)) {
	   hildon_banner_show_information(NULL, NULL,
	   "Host URI is not valid.");
	   return TRUE;
	   }
	 */

	if (!strlen(user)) {
		hildon_banner_show_information(NULL, NULL,
					       "Username cannot be empty.");
		return TRUE;
	}

	if (!strlen(pass)) {
		hildon_banner_show_information(NULL, NULL,
					       "Password cannot be empty.");
		return TRUE;
	}

	return FALSE;
}

static void _dialog_response_cb(GtkDialog * _dialog, gint response_id,
				gpointer user_data)
{
	(void)_dialog;
	cpa_dialog *dialog = user_data;
	GtkWindow *transient;

	if (response_id == GTK_RESPONSE_APPLY) {
		if (cpa_validate_settings(dialog))
			return;
		cpa_save_settings(dialog);
	} else if (response_id != GTK_RESPONSE_DELETE_EVENT) {
		g_debug("Wrong response ID! Some error has occurred.");
		return;
	}

	cpa_hide_ui(dialog);
	transient = gtk_window_get_transient_for(GTK_WINDOW(dialog->dialog));
	if (transient)
		gtk_widget_set_sensitive(GTK_WIDGET(transient), TRUE);

	gtk_widget_destroy(GTK_WIDGET(dialog->dialog));
	gtk_main_quit();
}

static cpa_dialog *cpa_dialog_new(GtkWindow * parent)
{
	cpa_dialog *dialog;

	GConfClient *gconf_client = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(gconf_client));

	dialog = g_try_new0(cpa_dialog, 1);

	dialog->pan = hildon_pannable_area_new();

	dialog->path_label = gtk_label_new("Local path for synchronisation");
	dialog->path_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
	hildon_entry_set_text(HILDON_ENTRY(dialog->path_entry),
			      gconf_client_get_string(gconf_client,
						      GCONF_KEY_NEXTCLOUD_PATH,
						      NULL));

	dialog->host_label = gtk_label_new("Host:");
	dialog->user_label = gtk_label_new("Username:");
	dialog->pass_label = gtk_label_new("Password:");

	dialog->host_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
	hildon_entry_set_text(HILDON_ENTRY(dialog->host_entry),
			      gconf_client_get_string(gconf_client,
						      GCONF_KEY_NEXTCLOUD_HOST,
						      NULL));

	dialog->user_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
	hildon_entry_set_text(HILDON_ENTRY(dialog->user_entry),
			      gconf_client_get_string(gconf_client,
						      GCONF_KEY_NEXTCLOUD_USER,
						      NULL));

	dialog->pass_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
	hildon_entry_set_text(HILDON_ENTRY(dialog->pass_entry),
			      gconf_client_get_string(gconf_client,
						      GCONF_KEY_NEXTCLOUD_PASS,
						      NULL));

	dialog->host_hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->host_hbox), dialog->host_label, TRUE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->host_hbox), dialog->host_entry, TRUE,
			   TRUE, 0);

	dialog->user_hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->user_hbox), dialog->user_label, TRUE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->user_hbox), dialog->user_entry, TRUE,
			   TRUE, 0);

	dialog->pass_hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->pass_hbox), dialog->pass_label, TRUE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->pass_hbox), dialog->pass_entry, TRUE,
			   TRUE, 0);

	dialog->autosync_check_btn =
	    hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
	gtk_button_set_label(GTK_BUTTON(dialog->autosync_check_btn),
			     "Enable automatic file sync on file changes");

	dialog->box = gtk_vbox_new(TRUE, 0);
	dialog->vbox = gtk_vbox_new(TRUE, 0);

	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->path_label, FALSE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->path_entry, FALSE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->autosync_check_btn,
			   FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->host_hbox, FALSE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->user_hbox, FALSE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), dialog->pass_hbox, FALSE,
			   FALSE, 0);

	gtk_box_pack_start(GTK_BOX(dialog->box), dialog->vbox, FALSE, FALSE, 0);

	dialog->dialog =
	    gtk_dialog_new_with_buttons("Nextcloud Sync Configuration",
					GTK_WINDOW(parent),
					GTK_DIALOG_NO_SEPARATOR |
					GTK_DIALOG_DESTROY_WITH_PARENT |
					GTK_DIALOG_MODAL, NULL);

	hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA
					       (dialog->pan), dialog->box);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog->dialog)->vbox),
			   dialog->pan, TRUE, TRUE, 0);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog->dialog), "Save",
			       GTK_RESPONSE_APPLY, NULL);

	g_signal_connect(dialog->dialog, "response",
			 G_CALLBACK(_dialog_response_cb), dialog);

	return dialog;
}

static gboolean cpa_state_load(gboolean * state_data, osso_context_t * osso)
{
	osso_state_t state;

	state.state_size = sizeof(*state_data);
	state.state_data = state_data;

	return osso_state_read(osso, &state) == OSSO_OK;
}

static gboolean cpa_update_ui(cpa_dialog * dialog)
{
	(void)dialog;
	return TRUE;
}

static void cpa_unhide_hide_ui(cpa_dialog * dialog)
{
	gtk_widget_show_all(dialog->pan);
	gtk_widget_set_size_request(dialog->pan, -1, 350);
	gtk_widget_show(dialog->dialog);
	gtk_widget_queue_resize(dialog->dialog);
}

static gint cpa_dialog_run(cpa_dialog * dialog)
{
	cpa_update_ui(dialog);

	if (!cpa_state_load(&dialog->state, dialog->osso))
		dialog->state = 0;

	if (dialog->state == 0) {
		cpa_unhide_hide_ui(dialog);
		return gtk_dialog_run(GTK_DIALOG(dialog->dialog));
	} else {
		return 1;
	}
}

static gboolean _idle_present_parent_window_cb(gpointer user_data)
{
	cpa_dialog *dialog = user_data;

	if (dialog) {
		cpa_dialog_run(dialog);
		gtk_window_present(GTK_WINDOW(dialog->parent));
	}

	return FALSE;
}

osso_return_t execute(osso_context_t * osso, gpointer data,
		      gboolean user_activated)
{
	(void)osso;
	(void)user_activated;
	GtkWindow *parent = (GtkWindow *) data;
	osso_return_t rv = OSSO_ERROR;

	if (parent) {
		cpa_dialog *dialog = cpa_dialog_new(parent);

		if (dialog) {
			dialog->parent = HILDON_WINDOW(parent);
			g_idle_add(_idle_present_parent_window_cb, dialog);
			gtk_main();
			rv = OSSO_OK;
		} else {
			g_critical("Unable to create applet dialog. Exiting.");
		}
	} else {
		g_critical("Missing data");
	}

	return rv;
}

osso_return_t save_state(osso_context_t * osso, gpointer data)
{
	(void)osso;
	(void)data;
	return OSSO_OK;
}
