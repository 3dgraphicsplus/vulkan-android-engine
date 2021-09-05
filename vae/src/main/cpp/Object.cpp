//
// Created by HuongNguyenXuan on 8/8/2021.
//

#include "Object.h"

Object::Object(const float vertices[], uint32_t size, uint32_t stride){

    LOGI("Object vertices size %d",size);
    this->vertices.assign(vertices,vertices+size);
    this->stride_ = stride;
}
