#ifndef __MS3D_LOADER_H__
#define __MS3D_LOADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;


// this from http://www.chumba.ch/chumbalum-soft//files/msViewer2.zip helper
// i change some code to c

/*
//
//                MilkShape 3D 1.4.0 File Format Specification
//
//                  This specifcation is written in C style.
//
// The data structures are defined in the order as they appear in the .ms3d file.
//

//
// max values
//
#define MAX_VERTICES    8192
#define MAX_TRIANGLES   16384
#define MAX_GROUPS      128
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed

//
// flags
//
#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8

//
// types
//
#ifndef byte
typedef unsigned char byte;
#endif // byte

#ifndef word
typedef unsigned short word;
#endif // word

//
// First comes the header.
//
typdef struct {
    char    id[10];                                     // always "MS3D000000"
    int     version;                                    // 3
} ms3d_header_t;

//
// Then comes the number of vertices
//
word nNumVertices;

//
// Then comes nNumVertices * sizeof (ms3d_vertex_t)
//
typedef struct {
    byte    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    float   vertex[3];                                  //
    char    boneId;                                     // -1 = no bone
    byte    referenceCount;
} ms3d_vertex_t;

//
// number of triangles
//
word nNumTriangles;

//
// nNumTriangles * sizeof (ms3d_triangle_t)
//
typedef struct {
    word    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    vertexIndices[3];                           //
    float   vertexNormals[3][3];                        //
    float   s[3];                                       //
    float   t[3];                                       //
    byte    smoothingGroup;                             // 1 - 32
    byte    groupIndex;                                 //
} ms3d_triangle_t;

//
// number of groups
//
word nNumGroups;

//
// nNumGroups * sizeof (ms3d_group_t)
//
typedef struct {
    byte            flags;                              // SELECTED | HIDDEN
    char            name[32];                           //
    word            numtriangles;                       //
    word            triangleIndices[numtriangles];      // the groups group the triangles
    char            materialIndex;                      // -1 = no material
} ms3d_group_t;

//
// number of materials
//
word nNumMaterials;

//
// nNumMaterials * sizeof (ms3d_material_t)
//
typedef struct {
    char            name[32];                           //
    float           ambient[4];                         //
    float           diffuse[4];                         //
    float           specular[4];                        //
    float           emissive[4];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[128];                       // texture.bmp
    char            alphamap[128];                      // alpha.bmp
} ms3d_material_t;

//
// save some keyframer data
//
float fAnimationFPS;
float fCurrentTime;
int iTotalFrames;

//
// number of joints
//
word nNumJoints;

//
// nNumJoints * sizeof (ms3d_joint_t)
//
typedef struct {
    float           time;                               // time in seconds
    float           rotation[3];                        // x, y, z angles
} ms3d_keyframe_rot_t;

typedef struct {
    float           time;                               // time in seconds
    float           position[3];                        // local position
} ms3d_keyframe_pos_t;

typedef struct {
    byte            flags;                              // SELECTED | DIRTY
    char            name[32];                           //
    char            parentName[32];                     //
    float           rotation[3];                        // local reference matrix
    float           position[3];

    word            numKeyFramesRot;                    //
    word            numKeyFramesTrans;                  //

    ms3d_keyframe_rot_t keyFramesRot[numKeyFramesRot];      // local animation matrices
    ms3d_keyframe_pos_t keyFramesTrans[numKeyFramesTrans];  // local animation matrices
} ms3d_joint_t;

//
// Mesh Transformation:
//
// 0. Build the transformation matrices from the rotation and position
// 1. Multiply the vertices by the inverse of local reference matrix (lmatrix0)
// 2. then translate the result by (lmatrix0 * keyFramesTrans)
// 3. then multiply the result by (lmatrix0 * keyFramesRot)
//
// For normals skip step 2.
//
// NOTE:  this file format may change in future versions!
//
// - Mete Ciragan
//
*/


namespace Ms3d_Space
{
// max values
#define MAX_VERTICES    8192
#define MAX_TRIANGLES   16384
#define MAX_GROUPS      128
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed

#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8

#define SPHEREMAP		0x80
#define HASALPHA		0x40
#define COMBINEALPHA    0x20

#define TRANSPARENCY_MODE_SIMPLE				0
#define TRANSPARENCY_MODE_DEPTHSORTEDTRIANGLES	1
#define TRANSPARENCY_MODE_ALPHAREF				2


#define eJointLines 0
#define eJointPoints 1

struct ms3d_header_t
{
    char    id[10];
    int     version;
} ;

struct ms3d_vertex_t
{
    unsigned char    flags;
    float   vertex[3];
    char    boneId;
    unsigned char    referenceCount;

    //========== 附加部分 ==========
    char boneIds[3];
	unsigned char weights[3];
	unsigned int extra;
	float renderColor[3];
} ;

struct ms3d_triangle_t
{
    unsigned short    flags;
    unsigned short    vertexIndices[3];
    float   vertexNormals[3][3];
    float   s[3];
    float   t[3];
    unsigned char    smoothingGroup;
    unsigned char    groupIndex;

    //========== 附加部分 ==========
    float normal[3];
} ;

struct ms3d_group_t
{
    unsigned char   flags;
    char            name[32];
    unsigned short  numtriangles;
    unsigned short* triangleIndices;
    char            materialIndex;
    //========== 附加部分 ==========
    size_t         numcomment;
    char*          comment;
} ;

struct ms3d_material_t
{
    char            name[32];
    float           ambient[4];
    float           diffuse[4];
    float           specular[4];
    float           emissive[4];
    float           shininess;
    float           transparency;
    char            mode;
    char            texture[128];
    char            alphamap[128];
    //========== 附加部分 ==========
    unsigned char   id;
    size_t          numcomment;
    char*           comment;
} ;

struct ms3d_keyframe_t
{
	float time;
	float key[3];
};

struct ms3d_tangent_t
{
	float tangentIn[3];
	float tangentOut[3];
};


struct ms3d_joint_t
{
    unsigned char   flags;
    char            name[32];
    char            parentName[32];
    float           rot[3];
    float           pos[3];

    unsigned short  numKeyFramesRot;
    unsigned short  numKeyFramesTrans;

    ms3d_keyframe_t* keyFramesRot;
    ms3d_keyframe_t* keyFramesTrans;

    //========== 附加部分 ==========
    ms3d_tangent_t* tangents;
    size_t numcomment;
    char* comment;
	float color[3];

	// used for rendering
	int parentIndex;
	float matLocalSkeleton[3][4];
	float matGlobalSkeleton[3][4];

	float matLocal[3][4];
	float matGlobal[3][4];
} ;


struct ms3d_model_t
{
    ms3d_header_t header;

    unsigned short nNumVertices;
    ms3d_vertex_t *vertexes;

    unsigned short nNumTriangles;
    ms3d_triangle_t *triangles;

    unsigned short nNumGroups;
    ms3d_group_t *groups;

    unsigned short nNumMaterials;
    ms3d_material_t *materials;

    float fAnimationFPS;
    float fCurrentTime;
    int iTotalFrames;

    unsigned short nNumJoints;
    ms3d_joint_t *joints;

    //=========== 附加部分 ================
    size_t  numcomment;
    char*   comment;

    float fjointize;
	int iTransparencyMode;
	float falphaRef;
} ;

ms3d_model_t* create_ms3d_model();
void delete_ms3d_model(ms3d_model_t* t);

bool load_ms3d_file(ms3d_model_t* t, const char* file);
void dump_ms3d_file(ms3d_model_t* t, const char* file);

unsigned int load_gl_tga(const char* file);
int find_joint_by_name(ms3d_model_t* t, const char *name);
void setup_joints(ms3d_model_t* t);
void setup_tangents(ms3d_model_t* t);
void set_frame(ms3d_model_t* t, float frame);
void evaluate_joint(ms3d_model_t* t, int index, float frame);

void transform_vertex(const ms3d_model_t* t, const ms3d_vertex_t *vertex, float out[3]);
void transform_normal(const ms3d_model_t* t, const ms3d_vertex_t *vertex, const float normal[3], float out[3]);
void fill_joint_indices_and_weights(const ms3d_vertex_t *vertex, int jointIndices[4], int jointWeights[4]);


bool gl_load_material(ms3d_model_t* t);
void gl_bind_material(const ms3d_model_t* t, int materialIndex);
void gl_render(const ms3d_model_t* t, bool withMaterial, bool flatShaded);
void gl_render_joints(const ms3d_model_t* t, int what);

class ModelRender
{
private:
    ms3d_model_t* m_model;
    string m_modelName;
    float m_frame;
    long m_clock;
public:
    ModelRender();
    ~ModelRender();

    float getFrameSecond();
    void loadModel(const char* filename);

    void renderModel();
    void renderJoints();

};

};

#endif // __MS3D_LOADER_H__
