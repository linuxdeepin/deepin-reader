target_sources(pdfium
    PRIVATE
        # Headers
        pdfium/fxjs/cjs_event_context_stub.h
        pdfium/fxjs/cjs_runtimestub.h
        pdfium/fxjs/cjs_event_context.h
        pdfium/fxjs/ijs_runtime.h

        # Source files
        pdfium/fxjs/cjs_event_context_stub.cpp
        pdfium/fxjs/cjs_runtimestub.cpp
        pdfium/fxjs/ijs_runtime.cpp
) 
