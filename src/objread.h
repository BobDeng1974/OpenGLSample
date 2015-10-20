#ifndef __OBJ_READ_H__
#define __OBJ_READ_H__

struct obj_idx3
{
    int v,vt,vn;
};
struct obj_vect3
{
    float x,y,z;
};
struct obj_uv
{
    float u,v;
};

struct obj_face
{
    obj_idx3 a,b,c;
};

struct obj_data_t
{
    unsigned short num_vertexs;
    unsigned short num_uv;
    unsigned short num_normal;
    unsigned short num_face;

    obj_vect3* v;
    obj_uv* vt;
    obj_vect3* vn;
    obj_face* f;

    char map_Kd[32];
    obj_vect3 Ka;
    obj_vect3 Kd;
    obj_vect3 Ks;
};

struct obj_render_t
{
    unsigned int num_index;
    float* v;
    float* vt;
    float* vn;
    unsigned int* indes;

    unsigned int tex;

    float Ka[4];
    float Kd[4];
    float Ks[4];
};

obj_data_t* obj_create();
void obj_destory(obj_data_t* t);

obj_render_t* obj_create_render(obj_data_t* l);
void obj_destory(obj_render_t* t);

void obj_read(const char* filename, obj_data_t* t);
void obj_render(obj_render_t* t);

#endif // __OBJ_READ_H__
