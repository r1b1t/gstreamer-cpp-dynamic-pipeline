#include "CreatePipeline.h"
#include <iostream>

// Kamera pipeline
GstElement *build_camera_pipeline()
{
    GstElement *pipeline, *src, *conv, *enc, *pay;

    pipeline = gst_pipeline_new("camera-pipe");
    src = gst_element_factory_make("ksvideosrc", "src");
    g_object_set(src, "device-index", 0, NULL);

    conv = gst_element_factory_make("videoconvert", "conv");
    enc = gst_element_factory_make("x264enc", "enc");
    g_object_set(enc, "tune", 4, "speed-preset", 1, "bitrate", 2000, NULL);
    pay = gst_element_factory_make("rtph264pay", "pay0");
    g_object_set(pay, "pt", 96, NULL);

    if (!pipeline || !src || !conv || !enc || !pay)
        return NULL;

    gst_bin_add_many(GST_BIN(pipeline), src, conv, enc, pay, NULL);
    if (!gst_element_link_many(src, conv, enc, pay, NULL))
    {
        g_printerr("Camera pipeline link error\n");
        gst_object_unref(pipeline);
        return NULL;
    }
    return pipeline;
}

// Test video pipeline
GstElement *build_test_pipeline()
{
    GstElement *pipeline, *src, *conv, *enc, *pay;

    pipeline = gst_pipeline_new("test-pipe");
    src = gst_element_factory_make("videotestsrc", "src");
    g_object_set(src, "is-live", TRUE, NULL);

    conv = gst_element_factory_make("videoconvert", "conv");
    enc = gst_element_factory_make("x264enc", "enc");
    g_object_set(enc, "tune", 4, "speed-preset", 1, "bitrate", 512, NULL);
    pay = gst_element_factory_make("rtph264pay", "pay0");
    g_object_set(pay, "pt", 97, NULL);

    if (!pipeline || !src || !conv || !enc || !pay)
        return NULL;

    gst_bin_add_many(GST_BIN(pipeline), src, conv, enc, pay, NULL);
    if (!gst_element_link_many(src, conv, enc, pay, NULL))
    {
        g_printerr("Test pipeline link error\n");
        gst_object_unref(pipeline);
        return NULL;
    }
    return pipeline;
}
