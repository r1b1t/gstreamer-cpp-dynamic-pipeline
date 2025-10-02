#include <gst/gst.h>

int main(int argc, char *argv[])
{
    GstElement *pipeline, *source, *filter, *convert, *sink;
    GstCaps *caps;
    GstBus *bus;
    GstMessage *msg;

    /* GStreamer'ı başlat */
    gst_init(&argc, &argv);

    /* Elemanları oluştur */
    source = gst_element_factory_make("ksvideosrc", "camera-source"); // Windows kamera
    filter = gst_element_factory_make("capsfilter", "filter");        // Format kısıtlayıcı
    convert = gst_element_factory_make("videoconvert", "converter");  // Format dönüştürücü
    sink = gst_element_factory_make("autovideosink", "video-output"); // Video çıkışı

    /* Pipeline oluştur */
    pipeline = gst_pipeline_new("camera-pipeline");

    if (!pipeline || !source || !filter || !convert || !sink)
    {
        g_printerr("Element oluşturulamadı.\n");
        return -1;
    }

    g_object_set(G_OBJECT(source),
                 "device-index", 0, // Bilgisayardaki hangi kamera (varsayılan 0)
                 "do-stats", TRUE,  // FPS, dropped frame gibi istatistikleri tut
                 NULL);

    /* Caps tanımı (640x480, 30 fps) */
    caps = gst_caps_new_simple("video/x-raw",
                               "width", G_TYPE_INT, 640,
                               "height", G_TYPE_INT, 480,
                               "framerate", GST_TYPE_FRACTION, 30, 1,
                               NULL);

    /* Capsfilter'a bağla */
    g_object_set(G_OBJECT(filter), "caps", caps, NULL);
    gst_caps_unref(caps); // capsi zaten tanımladığımız için artk ihtiyacımız kalmadı

    /* Pipeline’a ekle */
    gst_bin_add_many(GST_BIN(pipeline), source, filter, convert, sink, NULL);

    /* Elemanları sırayla bağla */
    if (!gst_element_link_many(source, filter, convert, sink, NULL))
    {
        g_printerr("Elementler bağlanamadı.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Pipeline’ı çalıştır */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Mesajları bekle (hata veya EOS) */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Temizlik */
    if (msg != NULL)
        gst_message_unref(msg);

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
