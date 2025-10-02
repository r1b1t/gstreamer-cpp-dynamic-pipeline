/*
Dynamic pipeline oluşturmak için GstRTSPMediaFactory’yi subclass’ladık ve create_element metodunu override ettik.
Override etmek = üst sınıftan gelen fonksiyonu kendi alt sınıfında yeniden yazmak.

Yeni bir sınıf (MyFactory) tanımladın.
Bu sınıf GstRTSPMediaFactory’den kalıtım aldı (yani MyFactory aslında bir GstRTSPMediaFactory’dir).
Kalıtım sayesinde GstRTSPMediaFactory’nin bütün özelliklerini ve fonksiyonlarını miras aldın.
Ama bir fonksiyonu (create_element) override ettin → böylece kendi pipeline kurma mantığını yazdın.

Yani:
Üst sınıf (GstRTSPMediaFactory): “RTSP server istemci bağlandığında bir pipeline yaratmam lazım” der.
Senin alt sınıfın (MyFactory): “Tamam, pipeline’ı ben şöyle kuracağım (kamera + encoder + RTP)” diyerek üst sınıfın davranışını değiştirir.
*/

#ifndef MY_FACTORY_H
#define MY_FACTORY_H

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
    gint pipeline_type;
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

// Sadece deklarasyon
GType my_factory_get_type(void);

#endif // MY_FACTORY_H
