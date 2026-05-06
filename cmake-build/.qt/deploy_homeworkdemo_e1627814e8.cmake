include("/Users/lansu/homeworkdemo/cmake-build/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/homeworkdemo-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "homeworkdemo.app"
)
