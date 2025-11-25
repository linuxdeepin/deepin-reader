target_sources(pdfium
    PRIVATE
        # Headers
        pdfium/third_party/agg23/agg_array.h
        pdfium/third_party/agg23/agg_basics.h
        pdfium/third_party/agg23/agg_clip_liang_barsky.h
        pdfium/third_party/agg23/agg_color_gray.h
        pdfium/third_party/agg23/agg_conv_adaptor_vcgen.h
        pdfium/third_party/agg23/agg_conv_dash.h
        pdfium/third_party/agg23/agg_conv_stroke.h
        pdfium/third_party/agg23/agg_curves.h
        pdfium/third_party/agg23/agg_math.h
        pdfium/third_party/agg23/agg_math_stroke.h
        pdfium/third_party/agg23/agg_path_storage.h
        pdfium/third_party/agg23/agg_pixfmt_gray.h
        pdfium/third_party/agg23/agg_rasterizer_scanline_aa.h
        pdfium/third_party/agg23/agg_render_scanlines.h
        pdfium/third_party/agg23/agg_renderer_base.h
        pdfium/third_party/agg23/agg_renderer_scanline.h
        pdfium/third_party/agg23/agg_rendering_buffer.h
        pdfium/third_party/agg23/agg_scanline_u.h
        pdfium/third_party/agg23/agg_shorten_path.h
        pdfium/third_party/agg23/agg_vcgen_dash.h
        pdfium/third_party/agg23/agg_vcgen_stroke.h
        pdfium/third_party/agg23/agg_vertex_sequence.h

        # Source files
        pdfium/third_party/agg23/agg_curves.cpp
        pdfium/third_party/agg23/agg_path_storage.cpp
        pdfium/third_party/agg23/agg_rasterizer_scanline_aa.cpp
        pdfium/third_party/agg23/agg_vcgen_dash.cpp
        pdfium/third_party/agg23/agg_vcgen_stroke.cpp
) 