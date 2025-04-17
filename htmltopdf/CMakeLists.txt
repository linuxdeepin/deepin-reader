# 定义htmltopdf可执行文件
add_executable(htmltopdf
    main.cpp
    htmltopdfconverter.cpp
    htmltopdfconverter.h
)

# 启用自动moc处理
set_target_properties(htmltopdf PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

find_package(Qt${QT_DESIRED_VERSION} REQUIRED COMPONENTS WebEngineWidgets)

# 链接Qt模块
target_link_libraries(htmltopdf PRIVATE
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    Qt${QT_VERSION_MAJOR}::Core
)

# 安全编译选项
target_compile_options(htmltopdf PRIVATE
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIC
    -pie
)

target_link_options(htmltopdf PRIVATE
    -pie
)

# 特殊架构优化
if(CMAKE_SYSTEM_PROCESSOR MATCHES "mips64")
    target_compile_options(htmltopdf PRIVATE
        -O3
        -ftree-vectorize
        -march=loongson3a
        -mhard-float
        -mno-micromips
        -mno-mips16
        -flax-vector-conversions
        -mloongson-ext2
        -mloongson-mmi
    )
endif()

# 安装目标
install(TARGETS htmltopdf
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/deepin-reader
)