add_library(LaurelEyePhysics STATIC)

add_library(LaurelEye::Physics ALIAS LaurelEyePhysics)

find_package(Bullet CONFIG REQUIRED)

# Normalize Bullet targets across package variants
set(LAURELEYE_BULLET_LIBS "")
if(TARGET BulletDynamics AND TARGET BulletCollision AND TARGET LinearMath)
    list(APPEND LAURELEYE_BULLET_LIBS BulletDynamics BulletCollision LinearMath)
elseif(TARGET Bullet::BulletDynamics AND TARGET Bullet::BulletCollision AND TARGET Bullet::LinearMath)
    list(APPEND LAURELEYE_BULLET_LIBS Bullet::BulletDynamics Bullet::BulletCollision Bullet::LinearMath)
else()
    message(FATAL_ERROR "Could not determine Bullet targets exposed by vcpkg.")
endif()

target_sources(LaurelEyePhysics PRIVATE
"CollisionManager.cpp"
"GhostBodyComponent.cpp"
"interfaces/Bullet/BulletGhostBody.cpp"
"interfaces/Bullet/BulletRigidBody.cpp"
"interfaces/Bullet/BulletShape.cpp"
"interfaces/Bullet/BulletWorld.cpp"
"PhysicsQueryAPI.cpp"
"PhysicsSystem.cpp"
"RigidBodyComponent.cpp"
)

target_precompile_headers(LaurelEyePhysics
    PRIVATE
        ${LAURELEYE_ROOT_DIR}/include/LaurelEyeEngine/pch.hpp
)

target_include_directories(LaurelEyePhysics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(LaurelEyePhysics
    PRIVATE
        LAURELEYE_PHYSICS_BUILD
)

set_target_properties(LaurelEyePhysics PROPERTIES
    OUTPUT_NAME "LaurelEyePhysics"
    FOLDER "Engine"
)

target_link_libraries(LaurelEyePhysics
    PUBLIC
        ${LAURELEYE_BULLET_LIBS}
)
