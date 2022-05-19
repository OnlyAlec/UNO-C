#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>

static GstElement *playbin;
static guintptr window_handle = 0;

static void report_error(GstBus *playbin_bus, GstMessage *error_message, gpointer user_data) {
    GError *error;
    gchar *debug_info;
    gst_message_parse_error(error_message, &error, &debug_info);
    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(error_message->src), error->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&error);
    g_free(debug_info);
}

static GstBusSyncReply prepare_window_handle(GstBus *bus, GstMessage *message, GstPipeline * pipeline) {
    if (!gst_is_video_overlay_prepare_window_handle_message(message)) {
        return GST_BUS_PASS;
    }
    g_assert(window_handle != 0);
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(playbin), window_handle);
    gst_message_unref(message);
    return GST_BUS_DROP;
}

static void realize_video_drawing_area(GtkWidget *video_drawing_area, gpointer user_data) {
    GdkWindow *video_window = gtk_widget_get_window(video_drawing_area);
    if (!gdk_window_ensure_native(video_window)) {
        g_error("Couldn't create native window needed for GstVideoOverlay!");
    }
    window_handle = (guintptr) GDK_WINDOW_HWND(video_window);
}

static void play(GtkButton *play_button, gpointer user_data) {
    gst_element_set_state(playbin, GST_STATE_PLAYING);
}

static void activate(GtkApplication *app, gpointer user_data) {

    GtkWidget *window = gtk_application_window_new(app);

    GtkWidget *root_pane = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(window), root_pane);

    GtkWidget *video_drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(root_pane), video_drawing_area);
    gtk_widget_set_size_request(video_drawing_area, 800, 600);
    gtk_widget_set_double_buffered(video_drawing_area, FALSE);
    g_signal_connect(video_drawing_area, "realize", G_CALLBACK(realize_video_drawing_area), NULL);

    GtkWidget *play_button = gtk_button_new_with_label("Play");
    gtk_overlay_add_overlay(GTK_OVERLAY(root_pane), play_button);
    gtk_widget_set_halign(play_button, GTK_ALIGN_END);
    gtk_widget_set_valign(play_button, GTK_ALIGN_END);
    gtk_widget_set_margin_end(play_button, 20);
    gtk_widget_set_margin_bottom(play_button, 20);
    g_signal_connect(G_OBJECT(play_button), "clicked", G_CALLBACK(play), NULL);

    gtk_widget_show_all(window);

}

int main(int argc, char **argv) {

    gst_init(&argc, &argv);
    playbin = gst_element_factory_make("playbin", NULL);
    g_object_set(playbin, "uri", "http://techslides.com/demos/sample-videos/small.ogv", NULL);
    GstBus *playbin_bus = gst_element_get_bus(playbin);
    gst_bus_enable_sync_message_emission(playbin_bus);
    gst_bus_set_sync_handler(playbin_bus, (GstBusSyncHandler) prepare_window_handle, NULL, NULL);
    gst_bus_add_signal_watch(playbin_bus);
    g_signal_connect(playbin_bus, "message::error", G_CALLBACK(report_error), NULL);
    gst_object_unref(playbin_bus);

    GtkApplication *app = gtk_application_new("com.techn.videotest", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);

}