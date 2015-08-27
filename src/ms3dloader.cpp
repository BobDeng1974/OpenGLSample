#include "ms3dloader.h"
#include "mathlib.h"


#include <GL/gl.h>
#include <GL/glu.h>


namespace Ms3d_Space
{

//======================================================
//
ms3d_model_t* create_ms3d_model()
{
    ms3d_model_t* p = new ms3d_model_t;
    memset(p, 0, sizeof(ms3d_model_t));
    return p;
}

//======================================================
//
void delete_ms3d_model(ms3d_model_t* t)
{
    if (t->vertexes)
    {
        delete[] t->vertexes;
    }
    if (t->triangles)
    {
        delete[] t->triangles;
    }
    if (t->groups)
    {
        for (unsigned short i = 0; i < t->nNumGroups; ++i)
        {
            ms3d_group_t* gt = &t->groups[i];
            if (gt->triangleIndices)
                delete[] gt->triangleIndices;
            if (gt->comment)
                delete[] gt->comment;
        }
        delete[] t->groups;
    }
    if (t->materials)
    {
        for (unsigned short i = 0; i < t->nNumMaterials; ++i)
        {
            ms3d_material_t* mt = &t->materials[i];
            if (mt->comment)
                delete[] mt->comment;
        }
        delete[] t->materials;
    }
    if (t->joints)
    {
        for (unsigned short i = 0; i < t->nNumJoints; ++i)
        {
            ms3d_joint_t* jt = &t->joints[i];
            if (jt->keyFramesRot)
                delete[] jt->keyFramesRot;
            if (jt->keyFramesTrans)
                delete[] jt->keyFramesTrans;
            if (jt->comment)
                delete[] jt->comment;
            if (jt->tangents)
                delete[] jt->tangents;
        }
        delete[] t->joints;
    }
    if (t->comment)
    {
        delete[] t->comment;
    }
}

//======================================================
//
bool load_ms3d_file(ms3d_model_t* t, const char* file)
{
    ms3d_header_t& header             = t->header;
    unsigned short& nNumVertices      = t->nNumVertices;
    unsigned short& nNumTriangles     = t->nNumTriangles;
    unsigned short& nNumGroups        = t->nNumGroups;
    unsigned short& nNumMaterials     = t->nNumMaterials;
    float& fAnimationFPS              = t->fAnimationFPS;
    float& fCurrentTime               = t->fCurrentTime;
    int& iTotalFrames                 = t->iTotalFrames;
    unsigned short& nNumJoints        = t->nNumJoints;

    FILE* fp = fopen(file, "rb");
    if (!fp)
        return false;

    // 文件大小
    fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

    // 读取头部
    fread(&header.id, 10, 1, fp);
    if (strncmp(header.id, "MS3D000000", 10) != 0)
	{
		fclose(fp);
		return false;
	}
	// 读取版本号
	fread(&header.version, sizeof(int), 1, fp);
	if (header.version != 4)
	{
		fclose(fp);
		return false;
	}
	// 读取顶点
    fread(&nNumVertices, sizeof(unsigned short), 1, fp);
	t->vertexes = new ms3d_vertex_t[nNumVertices];
	for (unsigned short i = 0; i < nNumVertices; ++i)
	{
		ms3d_vertex_t *vertex = &t->vertexes[i];

		fread(&vertex->flags, sizeof(unsigned char), 1, fp);
		fread(&vertex->vertex[0], sizeof(float), 1, fp);
		fread(&vertex->vertex[1], sizeof(float), 1, fp);
		fread(&vertex->vertex[2], sizeof(float), 1, fp);
		fread(&vertex->boneId, sizeof(char), 1, fp);
		fread(&vertex->referenceCount, sizeof(unsigned char), 1, fp);
	}

    // 读取三角形
    fread(&nNumTriangles, sizeof(unsigned short), 1, fp);
    t->triangles = new ms3d_triangle_t[nNumTriangles];
    for (unsigned short i = 0; i < nNumTriangles; ++i)
    {
        ms3d_triangle_t* triangle = &t->triangles[i];
        fread(&triangle->flags, sizeof(unsigned short), 1, fp);
		for (int j = 0; j < 3; ++j)
			fread(&triangle->vertexIndices[j], sizeof(unsigned short), 1, fp);
		for (int j = 0; j < 3; ++j)
		{
			fread(&triangle->vertexNormals[j][0], sizeof(float), 1, fp);
			fread(&triangle->vertexNormals[j][1], sizeof(float), 1, fp);
			fread(&triangle->vertexNormals[j][2], sizeof(float), 1, fp);
		}
		for (int j = 0; j < 3; ++j)
			fread(&triangle->s[j], sizeof(float), 1, fp);
		for (int j = 0; j < 3; ++j)
			fread(&triangle->t[j], sizeof(float), 1, fp);
		fread(&triangle->smoothingGroup, sizeof(unsigned char), 1, fp);
		fread(&triangle->groupIndex, sizeof(unsigned char), 1, fp);

		// TODO: calculate triangle normal
    }

    // 读取组
    fread(&nNumGroups, sizeof(unsigned short), 1, fp);
    t->groups = new ms3d_group_t[nNumGroups];
    for (unsigned short i = 0; i < nNumGroups; ++i)
    {
        ms3d_group_t* group = &t->groups[i];
        fread(&group->flags, sizeof(unsigned char), 1, fp);
        fread(group->name, 32, 1, fp);
        fread(&group->numtriangles, sizeof(unsigned short), 1, fp);
        group->triangleIndices = new unsigned short[group->numtriangles];
        for (int j = 0; j < group->numtriangles; ++j)
			fread(&group->triangleIndices[j], sizeof(unsigned short), 1, fp);
        fread(&group->materialIndex, sizeof(char), 1, fp);
    }

    // 读取材质
    fread(&nNumMaterials, sizeof(unsigned short), 1, fp);
    if (nNumMaterials > 0)
    {
        t->materials = new ms3d_material_t[nNumMaterials];
        for (unsigned short i = 0; i < nNumMaterials; ++i)
		{
		    ms3d_material_t *material = &t->materials[i];
            fread(material->name, 32, 1, fp);
            for (int j = 0; j < 4; ++j)
				fread(&material->ambient[j], sizeof(float), 1, fp);
			for (int j = 0; j < 4; ++j)
				fread(&material->diffuse[j], sizeof(float), 1, fp);
			for (int j = 0; j < 4; ++j)
				fread(&material->specular[j], sizeof(float), 1, fp);
			for (int j = 0; j < 4; ++j)
				fread(&material->emissive[j], sizeof(float), 1, fp);
			fread(&material->shininess, sizeof(float), 1, fp);
			fread(&material->transparency, sizeof(float), 1, fp);
			fread(&material->mode, sizeof(char), 1, fp);
			fread(material->texture, 128, 1, fp);
			fread(material->alphamap, 128, 1, fp);

            // set alpha
            material->ambient[3] = material->transparency;
            material->diffuse[3] = material->transparency;
            material->specular[3] = material->transparency;
            material->emissive[3] = material->transparency;
		}
    }

    // 保持一些数据
    fread(&fAnimationFPS, sizeof(float), 1, fp);
	fread(&fCurrentTime, sizeof(float), 1, fp);
	fread(&iTotalFrames, sizeof(int), 1, fp);
	fread(&nNumJoints, sizeof(unsigned short), 1, fp);

	if (nNumJoints > 0)
    {
        t->joints = new ms3d_joint_t[nNumJoints];
        for (unsigned short i = 0; i < nNumJoints; ++i)
        {
            ms3d_joint_t *joint = &t->joints[i];
            fread(&joint->flags, sizeof(unsigned char), 1, fp);
            fread(joint->name, 32, 1, fp);
            fread(joint->parentName, 32, 1, fp);
            fread(joint->rot, sizeof(float) * 3, 1, fp);
            fread(joint->pos, sizeof(float) * 3, 1, fp);

            fread(&joint->numKeyFramesRot, sizeof(unsigned short), 1, fp);
            fread(&joint->numKeyFramesTrans, sizeof(unsigned short), 1, fp);

            joint->keyFramesRot = new ms3d_keyframe_t[joint->numKeyFramesRot];
            for (unsigned short j = 0; j < joint->numKeyFramesRot; ++j)
			{
				ms3d_keyframe_t *frame = &joint->keyFramesRot[j];
				fread(&frame->time, sizeof(float), 1, fp);
				fread(&frame->key[1], sizeof(float), 1, fp);
				fread(&frame->key[0], sizeof(float), 1, fp);
				fread(&frame->key[2], sizeof(float), 1, fp);
			}

            joint->keyFramesTrans = new ms3d_keyframe_t[joint->numKeyFramesTrans];
            for (unsigned short j = 0; j < joint->numKeyFramesTrans; ++j)
			{
				ms3d_keyframe_t *frame = &joint->keyFramesTrans[j];
				fread(&frame->time, sizeof(float), 1, fp);
				fread(&frame->key[0], sizeof(float), 1, fp);
				fread(&frame->key[1], sizeof(float), 1, fp);
				fread(&frame->key[2], sizeof(float), 1, fp);
			}
        }
    }

    // comments
    long filePos = ftell(fp);
    if (filePos < fileSize)
    {
        int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 1)
		{
            int num = 0;
            int index;
            // group comments
			fread(&num, sizeof(int), 1, fp);
			for (int i = 0; i < num; ++i)
			{
				fread(&index, sizeof(int), 1, fp);
                ms3d_group_t* gt = &t->groups[index];
                fread(&gt->numcomment, sizeof(size_t), 1, fp);
                gt->comment = new char[gt->numcomment];
                fread(gt->comment, sizeof(char), gt->numcomment, fp);
			}

            // material comments
			fread(&num, sizeof(int), 1, fp);
			for (int i = 0; i < num; ++i)
			{
			    fread(&index, sizeof(int), 1, fp);
                ms3d_material_t* mt = &t->materials[index];
                fread(&mt->numcomment, sizeof(size_t), 1, fp);
                mt->comment = new char[mt->numcomment];
                fread(mt->comment, sizeof(char), mt->numcomment, fp);
			}

            // joint comments
            fread(&num, sizeof(int), 1, fp);
			for (int i = 0; i < num; ++i)
			{
			    fread(&index, sizeof(int), 1, fp);
                ms3d_joint_t* jt = &t->joints[index];
                fread(&jt->numcomment, sizeof(size_t), 1, fp);
                jt->comment = new char[jt->numcomment];
                fread(jt->comment, sizeof(char), jt->numcomment, fp);
			}

            // model comments
            fread(&num, sizeof(int), 1, fp);
            if (num == 1)
            {
                fread(&t->numcomment, sizeof(size_t), 1, fp);
                t->comment = new char[t->numcomment];
                fread(t->comment, sizeof(char), t->numcomment, fp);
			}
		}
		else
        {
            // "Unknown subversion for comments %d\n", subVersion);
        }
    }

    // vertex extra
	filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 2)
		{
			for (int i = 0; i < nNumVertices; ++i)
			{
				fread(&t->vertexes[i].boneIds[0], sizeof(char), 3, fp);
				fread(&t->vertexes[i].weights[0], sizeof(unsigned char), 3, fp);
				fread(&t->vertexes[i].extra, sizeof(unsigned int), 1, fp);
			}
		}
		else if (subVersion == 1)
		{
			for (int i = 0; i < nNumVertices; ++i)
			{
				fread(&t->vertexes[i].boneIds[0], sizeof(char), 3, fp);
				fread(&t->vertexes[i].weights[0], sizeof(unsigned char), 3, fp);
			}
		}
		else
		{
			// "Unknown subversion for vertex extra %d\n", subVersion);
		}
	}

	// joint extra
	filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 1)
		{
			for (int i = 0; i < nNumJoints; i++)
			{
				fread(&t->joints[i].color, sizeof(float), 3, fp);
			}
		}
		else
		{
			// "Unknown subversion for joint extra %d\n", subVersion);
		}
	}

    // model extra
	filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 1)
		{
			fread(&t->fjointize, sizeof(float), 1, fp);
			fread(&t->iTransparencyMode, sizeof(int), 1, fp);
			fread(&t->falphaRef, sizeof(float), 1, fp);
		}
		else
		{
			//"Unknown subversion for model extra %d\n", subVersion);
		}
	}


    fclose(fp);
    return true;
}


//======================================================
//
void dump_ms3d_file(ms3d_model_t* t, const char* file)
{
    FILE *fp = fopen(file, "w");
    fprintf(fp, "header:%s\n", t->header.id);
    fprintf(fp, "nNumVertices:%d\n", t->nNumVertices);
    fprintf(fp, "nNumTriangles:%d\n", t->nNumTriangles);
    fprintf(fp, "nNumGroups:%d\n", t->nNumGroups);
    fprintf(fp, "nNumMaterials:%d\n", t->nNumMaterials);
    fprintf(fp, "fAnimationFPS:%f\n", t->fAnimationFPS);
    fprintf(fp, "fCurrentTime:%f\n", t->fCurrentTime);
    fprintf(fp, "iTotalFrames:%d\n", t->iTotalFrames);
    fprintf(fp, "nNumJoints:%d\n", t->nNumJoints);

    fprintf(fp, "=============== vertexs ============\n");
    fflush(fp);
    for (unsigned short i = 0; i < t->nNumVertices; ++i)
    {
        ms3d_vertex_t* v = &t->vertexes[i];
        fprintf(fp, "flags:%d, x:%f,y:%f,z:%f, boneId:%d,referenceCount:%d \n", v->flags, v->vertex[0], v->vertex[1], v->vertex[2], v->boneId, v->referenceCount);
    }
    fprintf(fp, "=============== triangles ============\n");
    fflush(fp);
    for (unsigned short i = 0; i < t->nNumTriangles; ++i)
    {
        ms3d_triangle_t* tr = &t->triangles[i];
        fprintf(fp, "%d,(%d,%d,%d)[", tr->flags, tr->vertexIndices[0], tr->vertexIndices[1], tr->vertexIndices[2]);
        for (unsigned short j = 0; j < 3; ++j)
        {
            fprintf(fp, "%f,%f,%f", tr->vertexNormals[j][0], tr->vertexNormals[j][1], tr->vertexNormals[j][2]);
        }
        fprintf(fp, "]\n");
        fprintf(fp, "s[%f,%f,%f]t[%f,%f,%],%d,%d\n", tr->s[0], tr->s[1], tr->s[2], tr->t[0], tr->t[1], tr->t[2], tr->smoothingGroup, tr->groupIndex);
    }
    fprintf(fp, "=============== group ============\n");
    fflush(fp);
    for (unsigned short i = 0; i < t->nNumGroups; ++i)
    {
        ms3d_group_t* gt = &t->groups[i];
        fprintf(fp, "%s %d, %d, %d\n", gt->name, gt->flags, gt->numtriangles, gt->materialIndex);
        for (unsigned short j = 0; j < gt->numtriangles; ++j)
        {
            fprintf(fp, "%d,", gt->triangleIndices[j]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "=============== material ============\n");
    fflush(fp);
    for (unsigned short i = 0; i < t->nNumMaterials; ++i)
    {
        ms3d_material_t* tl = &t->materials[i];
        fprintf(fp, "%s\n", tl->name);
        fprintf(fp, "ambient: %f,%f,%f,%f\n", tl->ambient[0], tl->ambient[1], tl->ambient[2], tl->ambient[3]);
        fprintf(fp, "diffuse: %f,%f,%f,%f\n", tl->diffuse[0], tl->diffuse[1], tl->diffuse[2], tl->diffuse[3]);
        fprintf(fp, "specular: %f,%f,%f,%f\n", tl->specular[0], tl->specular[1], tl->specular[2], tl->specular[3]);
        fprintf(fp, "emissive: %f,%f,%f,%f\n", tl->emissive[0], tl->emissive[1], tl->emissive[2], tl->emissive[3]);
        fprintf(fp, "shininess: %f\n", tl->shininess);
        fprintf(fp, "transparency: %f\n", tl->transparency);
        fprintf(fp, "mode: %\n", tl->mode);
        fprintf(fp, "%s\n", tl->texture);
        fprintf(fp, "%s\n", tl->alphamap);
    }
    fprintf(fp, "=============== joint ============\n");
    fflush(fp);
    for (unsigned short i = 0; i < t->nNumJoints; ++i)
    {
        ms3d_joint_t* tt = &t->joints[i];
        fprintf(fp, "flags:%d, name: %s\n", tt->flags, tt->name);
        fprintf(fp, "parentName: %s\n", tt->parentName);

        fprintf(fp, "rot:%f,%f,%f\n", tt->rot[0], tt->rot[1], tt->rot[2]);
        fprintf(fp, "pos:%f,%f,%f\n", tt->pos[0], tt->pos[1], tt->pos[2]);

        fprintf(fp, "numKeyFramesRot:%d, numKeyFramesTrans:%d\n", tt->numKeyFramesRot, tt->numKeyFramesTrans);

        for (unsigned short j = 0; j < tt->numKeyFramesRot; ++j)
            fprintf(fp, "time:%f, x, y, z\n", tt->keyFramesRot[i].time, tt->keyFramesRot[i].key[0], tt->keyFramesRot[i].key[1], tt->keyFramesRot[i].key[2]);
        for (unsigned short j = 0; j < tt->numKeyFramesTrans; ++j)
            fprintf(fp, "time:%f, x, y, z\n", tt->keyFramesTrans[i].time, tt->keyFramesTrans[i].key[0], tt->keyFramesTrans[i].key[1], tt->keyFramesTrans[i].key[2]);
    }
}

//======================================================
//
int find_joint_by_name(ms3d_model_t* t, const char *name)
{
    for (unsigned short i = 0; i < t->nNumJoints; ++i)
	{
		if (strcmp(t->joints[i].name, name) == 0)
			return i;
	}

	return -1;
}

//======================================================
//
void setup_joints(ms3d_model_t* t)
{
    for (unsigned short i = 0; i < t->nNumJoints; ++i)
    {
        ms3d_joint_t* joint = &t->joints[i];
        joint->parentIndex = find_joint_by_name(t, joint->parentName);
    }

    // 对所有骨骼旋转平移成初始的姿势
    for (unsigned short i = 0; i < t->nNumJoints; ++i)
    {
        ms3d_joint_t* joint = &t->joints[i];
        AngleMatrix(joint->rot, joint->matLocalSkeleton);
        joint->matLocalSkeleton[0][3] = joint->pos[0];
        joint->matLocalSkeleton[1][3] = joint->pos[1];
        joint->matLocalSkeleton[2][3] = joint->pos[2];

        if (joint->parentIndex == -1)
        {
            memcpy(joint->matGlobalSkeleton, joint->matLocalSkeleton, sizeof(joint->matGlobalSkeleton));
        }
        else
        {
            ms3d_joint_t *parentJoint = &t->joints[joint->parentIndex];
            // 根据父骨骼的全局变换和当前骨骼的局部变换，计算出当前骨骼的全局变换
			R_ConcatTransforms(parentJoint->matGlobalSkeleton, joint->matLocalSkeleton, joint->matGlobalSkeleton);
        }
        setup_tangents(t);
    }

}

//======================================================
//
void setup_tangents(ms3d_model_t* t)
{
    for (unsigned short i = 0; i < t->nNumJoints; ++i)
    {
        ms3d_joint_t* joint = &t->joints[i];
        int numKeyFramesTrans = joint->numKeyFramesTrans;
        joint->tangents = new ms3d_tangent_t[numKeyFramesTrans];

        // clear all tangents (zero derivatives)
        for (int k = 0; k < numKeyFramesTrans; ++k)
        {
            joint->tangents[k].tangentIn[0] = 0.0f;
            joint->tangents[k].tangentIn[1] = 0.0f;
            joint->tangents[k].tangentIn[2] = 0.0f;

            joint->tangents[k].tangentOut[0] = 0.0f;
            joint->tangents[k].tangentOut[1] = 0.0f;
            joint->tangents[k].tangentOut[2] = 0.0f;
        }

        // if there are more than 2 keys,
        // we can calculate tangents,
        // otherwise we use zero derivatives
		if (numKeyFramesTrans > 2)
		{
            for (int k = 0; k < numKeyFramesTrans; ++k)
			{
				// make the curve tangents looped 循环计算曲线切线
				int k0 = k - 1;
				if (k0 < 0)
					k0 = numKeyFramesTrans - 1;
				int k1 = k;
				int k2 = k + 1;
				if (k2 >= numKeyFramesTrans)
					k2 = 0;

				// calculate the tangent, which is the vector from key[k - 1] to key[k + 1]
				float tangent[3];
				tangent[0] = (joint->keyFramesTrans[k2].key[0] - joint->keyFramesTrans[k0].key[0]);
				tangent[1] = (joint->keyFramesTrans[k2].key[1] - joint->keyFramesTrans[k0].key[1]);
				tangent[2] = (joint->keyFramesTrans[k2].key[2] - joint->keyFramesTrans[k0].key[2]);

				// weight the incoming and outgoing tangent by their time to
				// avoid changes in speed, if the keys are not within the same interval
				float dt1 = joint->keyFramesTrans[k1].time - joint->keyFramesTrans[k0].time;
				float dt2 = joint->keyFramesTrans[k2].time - joint->keyFramesTrans[k1].time;
				float dt = dt1 + dt2;
				joint->tangents[k1].tangentIn[0] = tangent[0] * dt1 / dt;
				joint->tangents[k1].tangentIn[1] = tangent[1] * dt1 / dt;
				joint->tangents[k1].tangentIn[2] = tangent[2] * dt1 / dt;

				joint->tangents[k1].tangentOut[0] = tangent[0] * dt2 / dt;
				joint->tangents[k1].tangentOut[1] = tangent[1] * dt2 / dt;
				joint->tangents[k1].tangentOut[2] = tangent[2] * dt2 / dt;
			}
		}

    }
}

//======================================================
//
void set_frame(ms3d_model_t* t, float frame)
{
    if (frame < 0.0f)
    {
        for (size_t i = 0; i < t->nNumJoints; ++i)
        {
            ms3d_joint_t* joint = &t->joints[i];
            memcpy(joint->matLocal, joint->matLocalSkeleton, sizeof(joint->matLocal));
            memcpy(joint->matGlobal, joint->matGlobalSkeleton, sizeof(joint->matGlobal));
        }
    }
    else
    {
        for (size_t i = 0; i < t->nNumJoints; i++)
		{
			evaluate_joint(t, i, frame);
		}
    }

    t->fCurrentTime = frame;
}

//======================================================
//
void evaluate_joint(ms3d_model_t* t, int index, float frame)
{
    ms3d_joint_t *jt = &t->joints[index];

    vec3_t pos = { 0.0f, 0.0f, 0.0f };
    int numKeyFramesTrans = jt->numKeyFramesTrans;
    if (numKeyFramesTrans > 0)
    {
        int i1 = -1;
        int i2 = -1;
        // 时间在两针之间的
        // find the two keys, where "frame" is in between for the position channel
        for (int i = 0; i < (numKeyFramesTrans - 1); ++i)
        {
            if (frame >= jt->keyFramesTrans[i].time && frame < jt->keyFramesTrans[i + 1].time)
            {
                i1 = i;
                i2 = i + 1;
                break;
            }
        }

        // if there are no such keys
        if (i1 == -1 || i2 == -1)
        {
            int index = -1;
            if (frame < jt->keyFramesTrans[0].time)
                index = 0;
            // or the last key
            else if (frame >= jt->keyFramesTrans[numKeyFramesTrans - 1].time)
                index = numKeyFramesTrans - 1;
            pos[0] = jt->keyFramesTrans[index].key[0];
            pos[1] = jt->keyFramesTrans[index].key[1];
            pos[2] = jt->keyFramesTrans[index].key[2];
        }
        else
        {
            ms3d_keyframe_t* p0 = &jt->keyFramesTrans[i1];
            ms3d_keyframe_t* p1 = &jt->keyFramesTrans[i2];
            ms3d_tangent_t* m0 = &jt->tangents[i1];
            ms3d_tangent_t* m1 = &jt->tangents[i2];

            // normalize the time between the keys into [0..1]
            float dt = (frame - jt->keyFramesTrans[i1].time) /
                (jt->keyFramesTrans[i2].time - jt->keyFramesTrans[i1].time);
            float t2 = dt * dt;
            float t3 = t2 * dt;

            // calculate hermite basis计算插值
            float h1 =  2.0f * t3 - 3.0f * t2 + 1.0f;
			float h2 = -2.0f * t3 + 3.0f * t2;
			float h3 =         t3 - 2.0f * t2 + dt;
			float h4 =         t3 -        t2;

            // do hermite interpolation
			pos[0] = h1 * p0->key[0] + h3 * m0->tangentOut[0] + h2 * p1->key[0] + h4 * m1->tangentIn[0];
			pos[1] = h1 * p0->key[1] + h3 * m0->tangentOut[1] + h2 * p1->key[1] + h4 * m1->tangentIn[1];
			pos[2] = h1 * p0->key[2] + h3 * m0->tangentOut[2] + h2 * p1->key[2] + h4 * m1->tangentIn[2];
        }
    }

    vec4_t quat = { 0.0f, 0.0f, 0.0f, 1.0f };
    int numKeyFramesRot = jt->numKeyFramesRot;
    if (numKeyFramesRot > 0)
    {
        int i1 = -1;
        int i2 = -1;
        // find the two keys, where "frame" is in between for the rotation channel
        for (int i = 0; i < (numKeyFramesRot - 1); ++i)
        {
            if (frame >= jt->keyFramesRot[i].time && frame < jt->keyFramesRot[i].time)
            {
                i1 = i;
                i2 = i + 1;
                break;
            }
        }

        // if there are no such key
        if (i1 == -1 || i2 == -1)
        {
            int index = -1;
            if (frame < jt->keyFramesRot[0].time)
                index = 0;
            else if (frame >= jt->keyFramesRot[numKeyFramesRot - 1].time)
                index = numKeyFramesRot - 1;
            AngleQuaternion(jt->keyFramesRot[index].key, quat);
        }
        // there are such keys, so do the quaternion slerp interpolation
        else
        {
            float dt = (frame - jt->keyFramesRot[i1].time) /
                (jt->keyFramesRot[i2].time - jt->keyFramesRot[i1].time);
                vec4_t q1;
                AngleQuaternion(jt->keyFramesRot[i1].key, q1);
                vec4_t q2;
                AngleQuaternion(jt->keyFramesRot[i2].key, q2);
                QuaternionSlerp(q1, q2, dt, quat);
        }
    }

    // make a matrix from pos/quat
    float matAnimate[3][4];
    QuaternionMatrix(quat, matAnimate);
    matAnimate[0][3] = pos[0];
    matAnimate[1][3] = pos[1];
    matAnimate[2][3] = pos[2];

    // animate the local joint matrix using: matLocal = matLocalSkeleton * matAnimate
    R_ConcatTransforms(jt->matLocalSkeleton, matAnimate, jt->matLocal);
    // build up the hierarchy if joints
	// matGlobal = matGlobal(parent) * matLocal
	if (jt->parentIndex == -1)
    {
        memcpy(jt->matGlobal, jt->matLocal, sizeof(jt->matGlobal));
    }
    else
    {
        ms3d_joint_t* parentJoint = &t->joints[jt->parentIndex];
        R_ConcatTransforms(parentJoint->matGlobal, jt->matLocal, jt->matGlobal);
    }

}

//======================================================
//
void transform_vertex(const ms3d_model_t* t, const ms3d_vertex_t *vertex, float out[3])
{
    int jointIndices[4], jointWeights[4];
    fill_joint_indices_and_weights(vertex, jointIndices, jointWeights);
    if (jointIndices[0] < 0 || jointIndices[0] >= (int)t->nNumJoints || t->fCurrentTime <= 0.0f)
    {
        out[0] = vertex->vertex[0];
        out[1] = vertex->vertex[1];
        out[2] = vertex->vertex[2];
    }
    else
    {
         // count valid weights
		int numWeights = 0;
		for (int i = 0; i < 4; ++i)
        {
            if (jointWeights[i] > 0 && jointIndices[i] >= 0 && jointIndices[i] < (int)t->nNumJoints)
                ++numWeights;
            else
                break;
        }

        // init
        out[0] = 0.0f;
        out[1] = 0.0f;
        out[2] = 0.0f;

        float weights[4] = {
            (float)jointWeights[0] / 100.0f,
            (float)jointWeights[1] / 100.0f,
            (float)jointWeights[2] / 100.0f,
            (float)jointWeights[3] / 100.0f};
        if (numWeights == 0)
        {
            numWeights = 1;
            weights[0] = 1.0f;
        }

        // add weighted vertices
        for (int i = 0; i < numWeights; ++i)
        {
            const ms3d_joint_t* jt = &t->joints[jointIndices[i]];
            vec3_t tmp, vert;
            VectorITransform(vertex->vertex, jt->matGlobalSkeleton, tmp);
            VectorRotate(tmp, jt->matGlobal, vert);

            out[0] += vert[0] * weights[i];
            out[1] += vert[1] * weights[i];
            out[2] += vert[2] * weights[i];
        }
    }
}

//======================================================
//
void transform_normal(const ms3d_model_t* t, const ms3d_vertex_t *vertex, const float normal[3], float out[3])
{
    int jointIndices[4], jointWeights[4];
    fill_joint_indices_and_weights(vertex, jointIndices, jointWeights);
    if (jointIndices[0] < 0 || jointIndices[0] >= (int)t->nNumJoints || t->fCurrentTime <= 0.0f)
    {
        out[0] = normal[0];
		out[1] = normal[1];
		out[2] = normal[2];
    }
    else
    {
        // count valid weights
		int numWeights = 0;
		for (int i = 0; i < 4; ++i)
        {
            if (jointWeights[i] > 0 && jointIndices[i] >= 0 && jointIndices[i] < (int)t->nNumJoints)
                ++numWeights;
            else
                break;
        }

        // init
        out[0] = 0.0f;
        out[1] = 0.0f;
        out[2] = 0.0f;

        float weights[4] = {
            (float)jointWeights[0] / 100.0f,
            (float)jointWeights[1] / 100.0f,
            (float)jointWeights[2] / 100.0f,
            (float)jointWeights[3] / 100.0f};
        if (numWeights == 0)
        {
            numWeights = 1;
            weights[0] = 1.0f;
        }

        // add weighted normals
        for (int i = 0; i < numWeights; ++i)
        {
            const ms3d_joint_t* jt = &t->joints[jointIndices[i]];
            vec3_t tmp, norm;
            VectorIRotate(normal, jt->matGlobalSkeleton, tmp);
            VectorRotate(tmp, jt->matGlobal, norm);

            out[0] += norm[0] * weights[i];
            out[1] += norm[1] * weights[i];
            out[2] += norm[2] * weights[i];
        }
    }
}

//======================================================
//
void fill_joint_indices_and_weights(const ms3d_vertex_t *vertex, int jointIndices[4], int jointWeights[4])
{
    jointIndices[0] = vertex->boneId;
    jointIndices[1] = vertex->boneIds[0];
    jointIndices[2] = vertex->boneIds[1];
    jointIndices[3] = vertex->boneIds[2];

    jointWeights[0] = 100;
    jointWeights[1] = 0;
    jointWeights[2] = 0;
    jointWeights[3] = 0;

    if (vertex->weights[0] != 0 || vertex->weights[1] != 0 || vertex->weights[2] != 0)
    {
        jointWeights[0] = vertex->weights[0];
        jointWeights[1] = vertex->weights[1];
        jointWeights[2] = vertex->weights[2];
        jointWeights[3] = 100 - (vertex->weights[0] + vertex->weights[1] + vertex->weights[2]);
    }
}


//======================================================
//
void bind_material(const ms3d_model_t* t, int materialIndex)
{
if (materialIndex < 0 || materialIndex >= t->nNumMaterials)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glColor4f(1, 1, 1, 1);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, 0);
		float ma[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		float md[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float ms[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float me[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float mss = 0.0f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ma);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, md);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ms);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, me);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mss);
	}
	else
	{
		const ms3d_material_t *material = &t->materials[materialIndex];
		glEnable(GL_TEXTURE_2D);

        if (material->transparency < 1.0f || material->mode & HASALPHA)
        {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, material->transparency);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

			if (t->iTransparencyMode == TRANSPARENCY_MODE_SIMPLE)
			{
				glDepthMask(GL_FALSE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.0f);
			}
			else if (t->iTransparencyMode == TRANSPARENCY_MODE_ALPHAREF)
			{
				glDepthMask(GL_TRUE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, t->falphaRef);
			}
        }
        else
        {
            glDisable(GL_BLEND);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
        }

		if (material->mode & SPHEREMAP)
		{
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
		else
		{
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
		glBindTexture(GL_TEXTURE_2D, material->id);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material->emissive);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
	}
}

//======================================================
//
void render_gl(const ms3d_model_t* t, bool withMaterial, bool flatShaded)
{
    int numGroups = t->nNumGroups;
	for (int i = 0; i < numGroups; i++)
	{
		const ms3d_group_t *group = &t->groups[i];

		if (withMaterial)
            bind_material(t, group->materialIndex);
		else
			bind_material(t, -1);

		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < group->numtriangles; j++)
		{
			const ms3d_triangle_t *triangle = &t->triangles[group->triangleIndices[j]];
			for (int v = 0; v < 3; v++)
			{
				const ms3d_vertex_t *vertex = &t->vertexes[triangle->vertexIndices[v]];

				glTexCoord2f(triangle->s[v], triangle->t[v]);

				float normal[3];
				if (flatShaded)
					transform_normal(t, vertex, triangle->normal, normal);
				else
					transform_normal(t, vertex, triangle->vertexNormals[v], normal);
				glNormal3fv(normal);

				float pos[3];
				transform_vertex(t, vertex, pos);
				glVertex3fv(pos);
			}
		}
		glEnd();
	}
}

//======================================================
//
void render_joints(const ms3d_model_t* t, int what)
{
    if (what == eJointLines)
	{
		glBegin(GL_LINES);
		for (int i = 0; i < t->nNumJoints; i++)
		{
			const ms3d_joint_t *joint = &t->joints[i];
			if (joint->parentIndex == -1)
			{
				glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
				glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
			}
			else
			{
				const ms3d_joint_t *parentJoint = &t->joints[joint->parentIndex];
				glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
				glVertex3f(parentJoint->matGlobal[0][3], parentJoint->matGlobal[1][3], parentJoint->matGlobal[2][3]);
			}
		}
		glEnd();
	}
	else if (what == eJointPoints)
	{
		glBegin(GL_POINTS);
		for (int i = 0; i < t->nNumJoints; i++)
		{
			const ms3d_joint_t *joint = &t->joints[i];
			glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
		}
		glEnd();
	}
}
};


