#include "MyFactory.h"

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    GstRTSPServer *server = gst_rtsp_server_new();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);

    // Kamera
    // Bizim factory
    /*Burada kendi yazdığın MyFactory tipinden yeni bir nesne oluşturuyorsun.
    g_object_new(my_factory_get_type(), NULL) → GObject sisteminde yeni instance yaratır
    static_cast<MyFactory*> → C++ tarzı cast
    Artık factory, her RTSP bağlantısı için pipeline yaratacak olan özel sınıfın.
    */
    MyFactory *cam_factory = (MyFactory *)g_object_new(my_factory_get_type(), NULL);
    cam_factory->pipeline_type = 0;
    gst_rtsp_mount_points_add_factory(mounts, "/camera", GST_RTSP_MEDIA_FACTORY(cam_factory));

    // Test video
    MyFactory *test_factory = (MyFactory *)g_object_new(my_factory_get_type(), NULL);
    test_factory->pipeline_type = 1;
    gst_rtsp_mount_points_add_factory(mounts, "/test", GST_RTSP_MEDIA_FACTORY(test_factory));

    g_object_unref(mounts);

    gst_rtsp_server_attach(server, NULL);
    g_print("RTSP server hazır: \n"
            "  rtsp://127.0.0.1:8554/camera\n"
            "  rtsp://127.0.0.1:8554/test\n");

    g_main_loop_run(loop);
    return 0;
}
