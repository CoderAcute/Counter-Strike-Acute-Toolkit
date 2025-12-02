#pragma once

#include"ElementBase/ElementBase.hpp"
#include"DerivedElements/FreeCameraPath/FreeCameraPath.hpp"
#include"DerivedElements/FirstPersonCameraPath/FirstPersonCameraPath.hpp"

//Element模板定义，元素必须继承自ElementBase
template<typename T>
concept Element = std::derived_from<T, ElementBase>;