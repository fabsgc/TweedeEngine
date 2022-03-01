# Find bullet
#
# This module defines
#  bullet_INCLUDE_DIRS
#  bullet_LIBRARIES
#  bullet_FOUND

start_find_package (bullet)

set (bullet_INSTALL_DIR ${APP_ROOT_DIR}/Dependencies/${PLATFORM_OS}.${PLATFORM_COMPILER}/bullet CACHE PATH "")
gen_default_lib_search_dirs (bullet)

find_imported_includes (bullet btBulletDynamicsCommon.h)

if(WIN32)
    find_imported_library(bullet LinearMath)
    find_imported_library(bullet Bullet3Collision)
    find_imported_library(bullet Bullet3Common)
    find_imported_library(bullet Bullet3Dynamics)
    find_imported_library(bullet Bullet3Geometry)
    find_imported_library(bullet BulletCollision)
    find_imported_library(bullet BulletDynamics)
    find_imported_library(bullet BulletSoftBody)
    find_imported_library(bullet BulletInverseDynamicsUtils)
    find_imported_library(bullet BulletInverseDynamics)
    find_imported_library(bullet ConvexDecomposition)
else()
    find_imported_library_shared(bullet LinearMath)
    find_imported_library_shared(bullet Bullet3Collision)
    find_imported_library_shared(bullet Bullet3Common)
    find_imported_library_shared(bullet Bullet3Dynamics)
    find_imported_library_shared(bullet Bullet3Geometry)
    find_imported_library_shared(bullet BulletCollision)
    find_imported_library_shared(bullet BulletDynamics)
    find_imported_library_shared(bullet BulletSoftBody)
    find_imported_library_shared(bullet BulletInverseDynamicsUtils)
    find_imported_library_shared(bullet BulletInverseDynamics)
    find_imported_library_shared(bullet ConvexDecomposition)
endif()

install_dependency_binaries(bullet)

end_find_package(bullet LinearMath)
end_find_package(bullet Bullet3Collision)
end_find_package(bullet Bullet3Common)
end_find_package(bullet Bullet3Dynamics)
end_find_package(bullet Bullet3Geometry)
end_find_package(bullet BulletCollision)
end_find_package(bullet BulletDynamics)
end_find_package(bullet BulletSoftBody)
end_find_package(bullet BulletInverseDynamicsUtils)
end_find_package(bullet BulletInverseDynamics)
end_find_package(bullet ConvexDecomposition)
