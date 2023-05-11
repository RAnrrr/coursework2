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

