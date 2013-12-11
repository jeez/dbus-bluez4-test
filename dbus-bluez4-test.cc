#include <gio/gio.h>

static GMainLoop *loop = NULL; // WILL LEAK
static GDBusProxy* manager_proxy_ = NULL; // WILL LEAK

static void OnGotDefaultAdapterPath(GObject*, GAsyncResult* res, gpointer user_data) {
  GError* error = 0;
  GVariant* result = g_dbus_proxy_call_finish(manager_proxy_, res, &error);

  if (!result) {
    g_printerr("\n\nError Got DefaultAdapter Path: %s\n", error->message);
    g_error_free(error);
    return;
  }

  char* path;
  g_variant_get(result, "(o)", &path);

  g_printerr("Got DefaultAdapter Path: %s\n", path);

  g_variant_unref(result);
  g_free(path);

  g_main_loop_quit(loop);
}

static void OnManagerCreated(GObject*, GAsyncResult* res, gpointer user_data) {
  GError* err = 0;
  manager_proxy_ = g_dbus_proxy_new_for_bus_finish(res, &err);

  if (!manager_proxy_) {
    g_printerr("## Manager Proxy creation error: %s\n", err->message);
    g_error_free(err);
    return;
  }

  g_dbus_proxy_call(manager_proxy_, "DefaultAdapter", NULL,
                    G_DBUS_CALL_FLAGS_NONE, 5000, NULL,
                    OnGotDefaultAdapterPath,
                    NULL);
}

int main(int argc, char** argv) {
#if !GLIB_CHECK_VERSION(2, 36, 0)
  // g_type_init() is deprecated on GLib since 2.36, Tizen has 2.32.
  g_type_init();
#endif

  g_printerr("\n\nStarting BlueZ4 SystemBus test\n");
  g_dbus_proxy_new_for_bus(G_BUS_TYPE_SYSTEM,
                             G_DBUS_PROXY_FLAGS_NONE,
                             NULL, /* GDBusInterfaceInfo */
                             "org.bluez",
                             "/",
                             "org.bluez.Manager",
                             NULL, /* GCancellable */
                             OnManagerCreated,
                             NULL);

  loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);
}

