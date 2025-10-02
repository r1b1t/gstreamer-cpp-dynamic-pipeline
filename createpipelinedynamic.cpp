/*
Dynamic pipeline oluşturmak için GstRTSPMediaFactory’yi subclass’ladın ve create_element metodunu override ettin.
Override etmek = üst sınıftan gelen fonksiyonu kendi alt sınıfında yeniden yazmak.

Yeni bir sınıf (MyFactory) tanımladın.
Bu sınıf GstRTSPMediaFactory’den kalıtım aldı (yani MyFactory aslında bir GstRTSPMediaFactory’dir).
Kalıtım sayesinde GstRTSPMediaFactory’nin bütün özelliklerini ve fonksiyonlarını miras aldın.
Ama bir fonksiyonu (create_element) override ettin → böylece kendi pipeline kurma mantığını yazdın.

Yani:
Üst sınıf (GstRTSPMediaFactory): “RTSP server istemci bağlandığında bir pipeline yaratmam lazım” der.
Senin alt sınıfın (MyFactory): “Tamam, pipeline’ı ben şöyle kuracağım (kamera + encoder + RTP)” diyerek üst sınıfın davranışını değiştirir.
*/

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

/*
instance struct’ı.
İçinde parent var → bu da aslında GstRTSPMediaFactory’den kalıtım yaptığımızı gösteriyor.
→Bu, new MyFactory() yaptığında bellekte oluşacak şey.
Yani MyFactory bir GstRTSPMediaFactory’dir (ama özel davranışlar ekleyebiliyoruz).
*/
typedef struct
{
    GstRTSPMediaFactory parent;
} MyFactory;

/*
class struct’ı.
MyFactoryClass adında bir struct tanımlıyoruz.
İçinde parent_class var → bu da üst sınıf olan GstRTSPMediaFactoryClass’ın bütün fonksiyonlarını miras alıyor.
Yani: MyFactoryClass, aslında GstRTSPMediaFactoryClass’ın bir alt sınıfı (subclass).
*/
typedef struct
{
    GstRTSPMediaFactoryClass parent_class;
} MyFactoryClass;

/*
GObject sistemine yeni bir sınıf kaydettik.
Bu sınıfın adı MyFactory, üst sınıfı GstRTSPMediaFactory.

Bu 3'ü otomatik oluşur
my_factory_class_init(MyFactoryClass *klass)
→ sınıf seviyesinde (class struct) ilk ayarları yaparsın (mesela metod override).

my_factory_init(MyFactory *factory)
→ nesne (instance) seviyesinde ilk ayarları yaparsın.

my_factory_get_type()
→ GObject tip sistemine bu yeni sınıfı kaydeden fonksiyon (makro üretir).
*/
G_DEFINE_TYPE(MyFactory, my_factory, GST_TYPE_RTSP_MEDIA_FACTORY);

static GstElement *
my_factory_create_element(GstRTSPMediaFactory *factory, const GstRTSPUrl *url)
{
    GstElement *pipeline, *src, *conv, *enc, *pay;

    pipeline = gst_pipeline_new("mypipe");

    src = gst_element_factory_make("ksvideosrc", "src");
    g_object_set(src,
                 "device-index", 0,
                 NULL);

    conv = gst_element_factory_make("videoconvert", "conv");

    enc = gst_element_factory_make("x264enc", "enc");
    g_object_set(enc,
                 "tune", 4,
                 "speed-preset", 1,
                 NULL);

    pay = gst_element_factory_make("rtph264pay", "pay0");
    g_object_set(pay,
                 "pt", 96,
                 NULL);

    if (!pipeline || !src || !conv || !enc || !pay)
    {
        g_printerr("Pipeline elemanları oluşturulamadı!\n");
        return NULL;
    }

    gst_bin_add_many(GST_BIN(pipeline), src, conv, enc, pay, NULL);
    if (!gst_element_link_many(src, conv, enc, pay, NULL))
    {
        g_printerr("Elementler bağlanamadı!\n");
        gst_object_unref(pipeline);
        return NULL;
    }

    return pipeline; // RTSP server bunu alıp GstRTSPMedia oluşturacak
}

/*
GObject sistemi her yeni tip tanımlandığında otomatik olarak çağırır.
Yani MyFactory sınıfı yüklenirken, bu fonksiyon bir kere çalışır.
*/
static void my_factory_class_init(MyFactoryClass *calass)
{
    /*
    Burada klass parametresini üst sınıfa cast(değişim yapmak) ediyorsun.
    calass senin MyFactoryClass tipin, ama onun içinde zaten GstRTSPMediaFactoryClass parent_class; vardı.
    Yani bu satır aslında “üst sınıfın metod tablosuna eriş” demek.
    */
    GstRTSPMediaFactoryClass *factory_class = GST_RTSP_MEDIA_FACTORY_CLASS(calass);
    /*
    Sen burada my_factory_create_element fonksiyonunu o pointer’a atayarak override etmiş oluyorsun.
    Artık bir istemci RTSP server’a bağlandığında create_element çağrılır → ama senin fonksiyonun çalışır, kendi pipeline’ını oluşturursun.
    */
    factory_class->create_element = my_factory_create_element;
}

static void my_factory_init(MyFactory *factory) {}

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    GstRTSPServer *server = gst_rtsp_server_new();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);

    // Bizim factory
    MyFactory *factory = static_cast<MyFactory *>(g_object_new(my_factory_get_type(), NULL));

    /*Burada kendi yazdığın MyFactory tipinden yeni bir nesne oluşturuyorsun.
    g_object_new(my_factory_get_type(), NULL) → GObject sisteminde yeni instance yaratır
    static_cast<MyFactory*> → C++ tarzı cast
    Artık factory, her RTSP bağlantısı için pipeline yaratacak olan özel sınıfın.
    */
    gst_rtsp_mount_points_add_factory(mounts, "/live", GST_RTSP_MEDIA_FACTORY(factory));
    g_object_unref(mounts);

    gst_rtsp_server_attach(server, NULL);
    g_print("RTSP server hazır: rtsp://127.0.0.1:8554/live\n");

    g_main_loop_run(loop);
    return 0;
}
