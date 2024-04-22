target_sources( ${PROJECT_NAME}
    PRIVATE
        bundle_instance.cpp
        canvas.cpp
        javascript_runtime.cpp
        main.cpp
        readfile.cpp
        renderer.cpp
        stb_image_write_instance.cpp
        stb_truetype_instance.cpp
        ${GENERATED_DIR}/bundle.h
)
