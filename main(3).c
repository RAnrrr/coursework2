
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INF 999999999
#define NUM 5000
// 定义一个结构体，用来保存解析出来的<node>数据
struct Node {
    int id;
    double lat;
    double lon;
};
// 定义一个结构体，用来保存解析出来的<link>数据
struct Edge {
    int node1;
    int node2;
    double length;
};
int Edge_NUM = 0;
int Node_NUM = 0;

int V = 0, S = 2000;  //V：start S:end

struct Node nodes[NUM];
struct Edge edges[NUM];

double Graph[NUM][NUM];
double Time_Graph[NUM][NUM];

//读取文件创建节点信息
void Create_Node(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Failed to open file '%s'\n", filename);
        return;
    }
    fseek(fp, 0, SEEK_SET); // 将文件指针定位到文件开头
    // 定义<node>标记的起始字符串和结束字符串
    char *start = "<node";
    char *end = "/node>";
    // 读取输入文件并解析<node>标记中的数据
    char buf[1024];
    int i = 0;
    while (fgets(buf, 1024, fp) != NULL) {
        // 如果行中包含<node>标记，就解析数据并保存到Node数组中
        char *ptr = strstr(buf, start);
        if (ptr != NULL) {
            char *endptr = strstr(ptr, end);
            if (endptr != NULL) {
                struct Node node;
                if (sscanf(ptr, "<node id=%ld lat=%lf lon=%lf", &node.id, &node.lat, &node.lon) == 3) {
//                    printf("id=%d lat=%f lon=%f\n", node.id, node.lat, node.lon);
                    nodes[i] = node;
                    i++;
                }
            }
        }
    }
    Node_NUM = i;
    fclose(fp);
}

//读取文件创建边的信息
void Create_Edge(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Failed to open file '%s'\n", filename);
        return;
    }
    fseek(fp, 0, SEEK_SET); // 将文件指针定位到文件开头
//    struct Edge edges[NUM];
    // 定义<node>标记的起始字符串和结束字符串
    char *start = "<link";
    char *end = "/link>";
    // 读取输入文件并解析<node>标记中的数据
    char buf[1024];
    int i = 0;
    while (fgets(buf, 1024, fp) != NULL) {
        // 如果行中包含<node>标记，就解析数据并保存到Node数组中
        char *ptr = strstr(buf, start);
        if (ptr != NULL) {
            char *endptr = strstr(ptr, end);
            if (endptr != NULL) {
                struct Edge edge;
                if (sscanf(ptr,
                           "<link id=%*d node=%d node=%d way=%*d length=%lf veg=%*f arch=%*f land=%*f POI=%*d;/link>",
                           &edge.node1, &edge.node2, &edge.length) == 3) {
//                    printf("node1=%d node2=%d length=%lf\n",edge.node1, edge.node2, edge.length);
                    edges[i] = edge;
                    i++;
                }
            }
        }
    }
    Edge_NUM = i;
    fclose(fp);
}

//对节点进行重新编码，对边的两个节点更新编码
void reshape_Node_Edge() {
    for (int i = 0; i < Node_NUM; i++) {
        int id = nodes[i].id;
        nodes[i].id = i;
        for (int j = 0; j < Edge_NUM; j++) {
            if (edges[j].node1 == id) {
                edges[j].node1 = i;
            }
            if (edges[j].node2 == id) {
                edges[j].node2 = i;
            }
        }
    }
//    for (int i = 0; i < Node_NUM; i++) {
//        printf("%d %lf %lf\n",nodes[i].id, nodes[i].lat, nodes[i].lon);
//    }
//    printf("\n\n\n");
//    for (int i = 0; i < Edge_NUM; i++) {
//        printf("%d %d %lf\n", edges[i].node1, edges[i].node2, edges[i].length);
//    }
}

//创建无向网
void Create_Undirected_Network() {
    for (int i = 0; i < NUM; i++) {
        for (int j = 0; j < NUM; j++) {
            Graph[i][j] = INF;
        }
    }
    for (int i = 0; i < Edge_NUM; i++) {
        int node1 = edges[i].node1;
        int node2 = edges[i].node2;
        Graph[node1][node2] = edges[i].length;
        Graph[node2][node1] = edges[i].length;
    }
    int num = 0;
    for (int i = 0; i < Node_NUM; i++) {
        for (int j = 0; j < Node_NUM; j++) {
            if (Graph[i][j] != INF) {
                num++;
            }

//            printf("%lf ,",Graph[i][j]);
        }
//        printf("\n");
    }
//    printf("num=%d",num);

}

//Dijkstra求最短路径
void Dijkstra() {
    int s[Node_NUM];
    double dist[Node_NUM];
    int path[Node_NUM];
    for (int i = 0; i < Node_NUM; i++) {
        dist[i] = Graph[V][i];
        s[i] = 0;
        if (dist[i] == INF)
            path[i] = -1;
        else
            path[i] = V;
    }
    s[V] = 1;
    int minv = -1;
    double minquan = INF;
    for (int i = 1; i < Node_NUM; i++) {
        for (int j = 0; j < Node_NUM; j++) {
            if (!s[j] && dist[j] < minquan) {
                minv = j;
                minquan = dist[j];
            }
        }
        s[minv] = 1;
        for (int j = 0; j < Node_NUM; j++) {
            if (Graph[minv][j] != INF && s[j] == 0 && dist[j] > minquan + Graph[minv][j]) {
                dist[j] = minquan + Graph[minv][j];
                path[j] = minv;
            }
        }
        minquan = 1000;
    }
    if (dist[S] == INF) {
        printf("最短路径不存在，存在孤立的点或者图没有连接无法到达\n");
        return;
    }
    int p = S;
    int count = 0;
    int result[Node_NUM];
    while (p != V) {
        result[count] = p;
        p = path[p];
        count++;
    }
    result[count] = V;
    printf("最短路径：");
    for (int i = count; i >= 0; i--) {
        if (i != 0)
            printf("%d->", result[i]);
        else
            printf("%d", result[i]);
    }
    printf("\n");
    printf("最短路径长度： %lf\n", dist[S]);
}

//创建时间权重无向图
void Create_Time_Undirected_Network() {
    for (int i = 0; i < NUM; i++) {
        for (int j = 0; j < NUM; j++) {
            Time_Graph[i][j] = INF;
        }
    }
    for (int i = 0; i < Edge_NUM; i++) {
        int node1 = edges[i].node1;
        int node2 = edges[i].node2;
        Time_Graph[node1][node2] = edges[i].length;
        Time_Graph[node2][node1] = edges[i].length;
    }
    double speed = 60;
    double limit_Speed = 30;
    for (int i = 0; i < Node_NUM; i++) {
        for (int j = 0; j < Node_NUM; j++) {
            if (Time_Graph[i][j] == INF || Time_Graph[i][j] == 0)
                continue;
            Time_Graph[i][j] = Time_Graph[i][j] / speed;
        }
    }
    int flag = 1;
    int node1, node2;
    char flags;
    while (flag) {
        printf("节点范围(0-%d) 输入道路限速节点node1和node2：", Node_NUM);
        fflush(stdout);  // 刷新缓冲区
        scanf("%d %d", &node1, &node2);
        if (Time_Graph[node1][node2] == INF || Time_Graph[node1][node2] == 0) {
            printf("当前道路不存在\n");
        } else {
            Time_Graph[node1][node2] /= limit_Speed;
            Time_Graph[node2][node1] /= limit_Speed;
            printf("道路%d到%d限速成功！！\n", node1, node2);
            fflush(stdout);  // 刷新缓冲区
        }
        printf("是否再次输入(y/n)：");
        fflush(stdout);  // 刷新缓冲区
        getchar();
        scanf("%c", &flags);
        if (flags == 'y')
            continue;
        else if (flags == 'n')
            flag = 0;
    }
}

//Time_Dijkstra求最快路径
void Time_Dijkstra() {
    int s[Node_NUM];
    double dist[Node_NUM];
    int path[Node_NUM];
    for (int i = 0; i < Node_NUM; i++) {
        dist[i] = Time_Graph[V][i];
        s[i] = 0;
        if (dist[i] == INF)
            path[i] = -1;
        else
            path[i] = V;
    }
    s[V] = 1;
    int minv = -1;
    double minquan = INF;
    for (int i = 1; i < Node_NUM; i++) {
        for (int j = 0; j < Node_NUM; j++) {
            if (!s[j] && dist[j] < minquan) {
                minv = j;
                minquan = dist[j];
            }
        }
        s[minv] = 1;
        for (int j = 0; j < Node_NUM; j++) {
            if (Time_Graph[minv][j] != INF && s[j] == 0 && dist[j] > minquan + Time_Graph[minv][j]) {
                dist[j] = minquan + Time_Graph[minv][j];
                path[j] = minv;
            }
        }
        minquan = 1000;
    }
    if (dist[S] == INF) {
        printf("最快路径不存在，存在孤立的点或者图没有连接无法到达\n");
        return;
    }
    int p = S;
    int count = 0;
    int result[Node_NUM];
    while (p != V) {
        result[count] = p;
        p = path[p];
        count++;
    }
    result[count] = V;
    printf("最快路径：");
    for (int i = count; i >= 0; i--) {
        if (i != 0)
            printf("%d->", result[i]);
        else
            printf("%d", result[i]);
    }
}

//经过一个特定点的Dijkstra算法求最短路径
void POI_Dijkstra(){
    int start,mid,end;
    printf("输入起点->关键点->终点:");
    fflush(stdout);  // 刷新缓冲区
    scanf("%d %d %d",&start,&mid,&end);
    V=start;
    S=mid;
    Dijkstra();
    V=mid;
    S=end;
    Dijkstra();
}

//编辑地图属性
void Edit_map_properties()
{
    int choice;
    while (1)
    {
        printf("节点范围(0-%d)\n",Node_NUM);
        printf("1:添加路径\n");
        printf("2:修改路径长度\n");
        printf("3:删除路径\n");
        printf("0:结束\n");
        printf("输入选择:");
        fflush(stdout);  // 刷新缓冲区
        scanf("%d",&choice);
        if(choice==0)
            break;
        else if(choice==1){
            int a,b;
            double length;
            printf("输入路径节点node1,node2:");
            fflush(stdout);  // 刷新缓冲区
            scanf("%d%d",&a,&b);
            if(Graph[a][b]!=INF){
                printf("已经存在该路径！！\n");
                fflush(stdout);  // 刷新缓冲区
                continue;
            }
            printf("路径长度:");
            fflush(stdout);  // 刷新缓冲区
            scanf("%lf",&length);
            Graph[a][b]=length;
            Graph[b][a]=length;
            printf("路径添加成功!!\n");
            fflush(stdout);  // 刷新缓冲区
        } else if(choice==2){
            int a,b;
            double length;
            printf("输入路径节点node1,node2:");
            fflush(stdout);  // 刷新缓冲区
            scanf("%d%d",&a,&b);
            if(Graph[a][b]==INF){
                printf("不存在该路径！！\n");
                fflush(stdout);  // 刷新缓冲区
                continue;
            }
            printf("修改路径长度:");
            fflush(stdout);  // 刷新缓冲区
            scanf("%lf",&length);
            Graph[a][b]=length;
            Graph[b][a]=length;
            printf("路径长度修改成功!!\n");
            fflush(stdout);  // 刷新缓冲区
        } else if(choice==3){
            int a,b;
            printf("输入路径节点node1,node2:");
            fflush(stdout);  // 刷新缓冲区
            scanf("%d%d",&a,&b);
            if(Graph[a][b]==INF){
                printf("不存在该路径！！\n");
                fflush(stdout);  // 刷新缓冲区
                continue;
            }
            Graph[a][b]=INF;
            Graph[b][a]=INF;
            printf("路径删除成功!!\n");
            fflush(stdout);  // 刷新缓冲区
        }
    }
}

//功能菜单
int menu(){
    int choice;
    printf("功能列表\n");
    printf("1:最短路径\n");
    printf("2:最快路径\n");
    printf("3:关键点路径\n");
    printf("4:编辑和添加地图属性\n");
    printf("0:结束\n");
    printf("输入功能选择:");
    fflush(stdout);  // 刷新缓冲区
    scanf("%d",&choice);
    return choice;
}


int main() {
    char *filename = "C:\\Users\\wuxiaodi\\CLionProjects\\map\\leeds.txt";
    Create_Edge(filename);
    Create_Node(filename);
//    for (int i = 0; i < Node_NUM; i++) {
////        printf("%d %lf %lf\n", nodes[i].id, nodes[i].lat, nodes[i].lon);
//    }
//    printf("节点数:%d",Node_NUM);
//    printf("\n\n\n");
//    for (int i = 0; i < Edge_NUM; i++) {
////        printf("%d %d %lf\n", edges[i].node1, edges[i].node2, edges[i].length);
//    }
//    printf("边数:%d",Edge_NUM);


    //对节点进行重新编码，对边的两个节点更新编码
    reshape_Node_Edge();
    Create_Undirected_Network();
    int choice;
    while (1){
        choice=menu();
        if(choice==0)
            break;
        else if(choice==1){
            printf("输入V和S:");
            fflush(stdout);  // 刷新缓冲区
            scanf("%d %d",&V,&S);
            Dijkstra();
        } else if(choice==2){
            Create_Time_Undirected_Network();
            Time_Dijkstra();
        } else if(choice==3){
            POI_Dijkstra();
        } else if(choice==4){
            Edit_map_properties();
        }
    }
    return 0;
}
