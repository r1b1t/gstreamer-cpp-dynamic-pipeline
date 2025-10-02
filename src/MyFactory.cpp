#include "MyFactory.h"
#include "CreatePipeline.h"

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

static GstElement *my_factory_create_element(GstRTSPMediaFactory *factory, const GstRTSPUrl *url)
{
    (void)url;
    /*
    Eğer MyFactory struct’ına kendi alanlarını (örn. int pipeline_type; char *file_path;) eklersen,
    create_element içinde bunlara erişmek için MyFactory* tipine ihtiyacın olacak.
    GStreamer sana hep GstRTSPMediaFactory* factory parametresi gönderiyor. Bu üst sınıf.
    Senin özel alanlarını göremiyor.
    İşte burada MyFactory *self = (MyFactory*) factory; yazarak üst sınıf pointerını kendi alt sınıf pointerına dönüştürüyorsun.
    */
    MyFactory *self = (MyFactory *)factory;

    if (self->pipeline_type == 0)
        return build_camera_pipeline();
    else if (self->pipeline_type == 1)
        return build_test_pipeline();

    return NULL;
}

/*
GObject sistemi her yeni tip tanımlandığında otomatik olarak çağırır.
Yani MyFactory sınıfı yüklenirken, bu fonksiyon bir kere çalışır.
*/
static void my_factory_class_init(MyFactoryClass *klass)
{
    /*
    Burada klass parametresini üst sınıfa cast(değişim yapmak) ediyorsun.
    calass senin MyFactoryClass tipin, ama onun içinde zaten GstRTSPMediaFactoryClass parent_class; vardı.
    Yani bu satır aslında “üst sınıfın metod tablosuna eriş” demek.
    */
    GstRTSPMediaFactoryClass *factory_class = GST_RTSP_MEDIA_FACTORY_CLASS(klass);
    /*
    Sen burada my_factory_create_element fonksiyonunu o pointer’a atayarak override etmiş oluyorsun.
    Artık bir istemci RTSP server’a bağlandığında create_element çağrılır → ama senin fonksiyonun çalışır, kendi pipeline’ını oluşturursun.
    */
    factory_class->create_element = my_factory_create_element;
}

static void my_factory_init(MyFactory *factory)
{
    factory->pipeline_type = 0;
}
