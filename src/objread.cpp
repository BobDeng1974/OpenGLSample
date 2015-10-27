
#include "objread.h"
#include "datalib.h"
#include "texture.h"


//===================================

obj_data_t* obj_create()
{
    obj_data_t *p = new obj_data_t();
    memset(p, 0, sizeof(obj_data_t));
    return p;
}

void obj_destory_data(obj_data_t* t)
{
    assert(t != NULL);
    delete[] (t->v);
    delete[] (t->vt);
    delete[] (t->vn);
    delete[] (t->f);
    delete t;
}


inline void set_vertex_data(unsigned int i, obj_render_t* p, obj_vect3* v, obj_uv* vt, obj_vect3* vn)
{
    unsigned int j = i * 3;
    unsigned int jt = i * 2;

    p->v[j] = v->x;
    p->v[j + 1] = v->y;
    p->v[j + 2] = v->z;

    p->vt[jt] = vt->u;
    p->vt[jt + 1] = vt->v;

    p->vn[j] = vn->x;
    p->vn[j + 1] = vn->y;
    p->vn[j + 2] = vn->z;
}

obj_render_t* obj_create_render(obj_data_t* l)
{
    obj_render_t* p = new obj_render_t();

    unsigned short num_vertexs = l->num_vertexs;
    unsigned short num_face = l->num_face;

    p->v = new float[num_face * 3 * 3];
    p->vt = new float[num_face * 3 * 2];
    p->vn = new float[num_face * 3 * 3];

    p->num_index = num_face * 3;
    p->indes = new unsigned int[num_face * 3];
    for (unsigned short  i = 0; i < p->num_index; ++i)
        p->indes[i] = i;

    for (unsigned short i = 0; i < num_face; ++i)
    {
        obj_face* f = &l->f[i];

        obj_idx3& a = f->a;
        obj_idx3& b = f->b;
        obj_idx3& c = f->c;

        obj_vect3* v1 = &l->v[a.v - 1];
        obj_vect3* v2 = &l->v[b.v - 1];
        obj_vect3* v3 = &l->v[c.v - 1];

        obj_uv* vt1 = &l->vt[a.vt - 1];
        obj_uv* vt2 = &l->vt[b.vt - 1];
        obj_uv* vt3 = &l->vt[c.vt - 1];

        obj_vect3* vn1 = &l->vn[a.vn - 1];
        obj_vect3* vn2 = &l->vn[b.vn - 1];
        obj_vect3* vn3 = &l->vn[c.vn - 1];

        set_vertex_data(i * 3, p, v1, vt1, vn1);
        set_vertex_data(i * 3 + 1, p, v2, vt2, vn2);
        set_vertex_data(i * 3 + 2, p, v3, vt3, vn3);
    }

    char path[128] = {0};
    sprintf(path, "%s/%s", l->data_path, l->map_Kd);
    p->tex = LoadGLBitmap24(path);
    memcpy(p->Ka, &l->Ka, sizeof(obj_vect3));
    memcpy(p->Kd, &l->Kd, sizeof(obj_vect3));
    memcpy(p->Ks, &l->Ks, sizeof(obj_vect3));
    p->Ka[3] = p->Kd[3] = p->Ks[3] = 1.0f;

    return p;
}

void obj_destory_render(obj_render_t* t)
{
    assert(t != NULL);

    glDeleteTextures(1, &t->tex);

    delete[] (t->v);
    delete[] (t->vt);
    delete[] (t->vn);
    delete[] (t->indes);
    delete t;
}

void obj_render_dump(obj_render_t* t)
{
    unsigned int num_index = t->num_index ;
    unsigned int num = t->num_index * 3;
    printf("----------- vetex ---------------");
    for (int i = 0; i < num_index * 3; ++i)
    {
        if (i % 3 == 0)
            printf("\n");
        printf("%f,", t->v[i]);
    }
    printf("\n----------- normal ---------------");
    for (int i = 0; i < num_index * 3; ++i)
    {
        if (i % 3 == 0)
            printf("\n");
        printf("%f,", t->vn[i]);
    }

    printf("\n----------- texture ---------------");
    for (int i = 0; i < num_index * 2; ++i)
    {
        if (i % 2 == 0)
            printf("\n");
        printf("%f,", t->vt[i]);
    }

    printf("\n----------- face ---------------");
    for (int i = 0; i < num_index; ++i)
    {
        if (i % 3 == 0)
            printf("\n");
        printf("%d,", t->indes[i]);
    }
}

void obj_read(const char* obj, const char* mtl, obj_data_t* t)
{
    char buff[256];
    FILE *ofp = fopen(obj, "r");

    st_node* vt_head = create_node();
    st_node* vn_head = create_node();
    st_node* v_head = create_node();
    st_node* f_head = create_node();

    st_node* vt_tmp = vt_head;
    st_node* vn_tmp = vn_head;
    st_node* v_tmp = v_head;
    st_node* f_tmp = f_head;

    int vt_num = 0;
    int vn_num = 0;
    int v_num = 0;
    int f_num = 0;

    while (fgets(buff, 256, ofp) != NULL)
    {
        if (memcmp(buff, "vt", 2) == 0)
        {
            vt_num++;
            obj_uv* v = new obj_uv();
            sscanf(buff + 3, "%f %f", &v->u, &v->v);
            vt_tmp = add_node(vt_tmp, v);
        }
        else if (memcmp(buff, "vn", 2) == 0)
        {
            vn_num++;
            obj_vect3* v = new obj_vect3();
            sscanf(buff + 3, "%f %f %f", &v->x, &v->y, &v->z);
            vn_tmp = add_node(vn_tmp, v);
        }
        else if (memcmp(buff, "v ", 2) == 0)
        {
            v_num++;
            obj_vect3* v = new obj_vect3();
            sscanf(buff + 2, "%f %f %f", &v->x, &v->y, &v->z);
            v_tmp = add_node(v_tmp, v);
        }
        else if (memcmp(buff, "f ", 2) == 0)
        {
            f_num++;
            obj_face* v = new obj_face();
            sscanf(buff + 2, "%d/%d/%d %d/%d/%d %d/%d/%d",
                   &v->a.v, &v->a.vt, &v->a.vn,
                   &v->b.v, &v->b.vt, &v->b.vn,
                   &v->c.v, &v->c.vt, &v->c.vn);
            f_tmp = add_node(f_tmp, v);
        }
    }
    fclose(ofp);

    // �洢���ṹ������
    t->num_vertexs = v_num;
    t->num_uv = vt_num;
    t->num_normal = vn_num;
    t->num_face = f_num;

    t->v = new obj_vect3[v_num];
    t->vt = new obj_uv[vt_num];
    t->vn = new obj_vect3[vn_num];
    t->f = new obj_face[f_num];

    v_tmp = (st_node*)v_head->next;
    vt_tmp = (st_node*)vt_head->next;
    vn_tmp = (st_node*)vn_head->next;
    f_tmp = (st_node*)f_head->next;

    st_node* del_tmp = NULL;

    int i = 0;
    while (v_tmp != NULL)
    {
        memcpy(&t->v[i], v_tmp->data, sizeof(obj_vect3));
        del_tmp = v_tmp;
        v_tmp = v_tmp->next;
        delete del_tmp->data;
        i++;
    }

    i = 0;
    while (vt_tmp != NULL)
    {
        memcpy(&t->vt[i], vt_tmp->data, sizeof(obj_uv));
        del_tmp = vt_tmp;
        vt_tmp = vt_tmp->next;
        delete del_tmp->data;
        i++;
    }

    i = 0;
    while (vn_tmp != NULL)
    {
        memcpy(&t->vn[i], vn_tmp->data, sizeof(obj_vect3));
        del_tmp = vn_tmp;
        vn_tmp = vn_tmp->next;
        delete del_tmp->data;
        i++;
    }

    i = 0;
    while (f_tmp != NULL)
    {
        memcpy(&t->f[i], f_tmp->data, sizeof(obj_face));
        del_tmp = f_tmp;
        f_tmp = f_tmp->next;
        delete del_tmp->data;
        i++;
    }

    remove_nodes(v_head);
    remove_nodes(vt_head);
    remove_nodes(vn_head);
    remove_nodes(f_head);

    FILE *mfp = fopen(mtl, "r");
    while (fgets(buff, 256, mfp) != NULL)
    {
        if (memcmp(buff, "Ka", 2) == 0)
        {
            sscanf(buff + 2, "%f %f %f", &t->Ka.x, &t->Ka.y, &t->Ka.z);
        }
        else if (memcmp(buff, "Kd", 2) == 0)
        {
            sscanf(buff + 2, "%f %f %f", &t->Kd.x, &t->Kd.y, &t->Kd.z);
        }
        else if (memcmp(buff, "Ks", 2) == 0)
        {
            sscanf(buff + 2, "%f %f %f", &t->Ks.x, &t->Ks.y, &t->Ks.z);
        }
        else if (memcmp(buff, "map_Kd", 6) == 0)
        {
            memcpy(t->map_Kd, buff + 7, 32);
            i = strlen(t->map_Kd);
            while (i > 0)
            {
                if (t->map_Kd[i] == '\n')
                    t->map_Kd[i] = 0;
                i--;
            }
        }
        memset(buff, 0, 256);
    }
    fclose(mfp);

    // ����һ�»�ȡ���յ�Ŀ¼
    size_t sz = strlen(mtl);
    i = sz - 1;
    while (i > 0)
    {
        if (mtl[i] == '/' || mtl[i] == '\\')
            break;
        i--;
    }
    // ����·��
    if (i > 0)
    {
        memcpy(t->data_path, mtl, i);
    }
}


void obj_render(obj_render_t* t)
{
    glEnable(GL_CULL_FACE);     // �������޳�
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER ,0.9);//0.5���Ի����κ���0~1֮�����
    glShadeModel(GL_SMOOTH);

    const GLfloat high_shininess[] = { 100.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT,   t->Ka);   // ���û�������ɫ
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   t->Kd);   // �������������ɫ
    glMaterialfv(GL_FRONT, GL_SPECULAR,  t->Ks);  // ���þ��淴�����ɫ
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t->tex);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, t->v);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, t->vt);

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, t->vn);

    //glEnableClientState(GL_COLOR_ARRAY);
    //glColorPointer(3, GL_FLOAT, 0, t->colors);

    glDrawElements(GL_TRIANGLES, t->num_index, GL_UNSIGNED_INT, t->indes);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
