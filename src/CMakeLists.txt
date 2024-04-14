target_sources( ${PROJECT_NAME}
    PRIVATE
        javascript_runtime.cpp
        main.cpp
        stb_image_write_instance.cpp
        stb_truetype_instance.cpp
        ${GENERATED_DIR}/bundle.h
)
