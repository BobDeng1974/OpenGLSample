#include "ms3dloader.h"
#include "mathlib.h"



namespace Ms3d_Space
{

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

int find_joint_by_name(ms3d_model_t* t, const char *name)
{
    for (unsigned short i = 0; i < t->nNumJoints; ++i)
	{
		if (strcmp(t->joints[i].name, name) == 0)
			return i;
	}

	return -1;
}

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
				// make the curve tangents looped
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

void evaluate_joint(ms3d_model_t* t, int index, float frame)
{
    ms3d_joint_t *joint = &t->joints[index];
    // TODO
    vec3_t pos = { 0.0f, 0.0f, 0.0f };
    int numKeyFramesTrans = joint->numKeyFramesTrans;
    if (numKeyFramesTrans > 0)
    {
        int i1 = -1;
        int i2 = -1;
    }
}

void transform_vertex(const ms3d_vertex_t *vertex, float out[3]);
void transform_normal(const ms3d_vertex_t *vertex, const float normal[3], float out[3]){}
void fill_joint_indices_and_weights(const ms3d_vertex_t *vertex, int jointIndices[4], int jointWeights[4]){}


};


