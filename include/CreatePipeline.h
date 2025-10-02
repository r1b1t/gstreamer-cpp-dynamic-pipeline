#ifndef CREATE_PIPELINE_H
#define CREATE_PIPELINE_H

#include <gst/gst.h>

// Kamera pipeline
GstElement *build_camera_pipeline();

// Test video pipeline
GstElement *build_test_pipeline();

#endif // CREATE_PIPELINE_H
