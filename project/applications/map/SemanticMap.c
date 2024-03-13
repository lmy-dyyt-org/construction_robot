// SemanticMap.cpp: 定义应用程序的入口点。
#include "SemanticMap.h"
#include "stdio.h"
/**
 * @brief 本算法规定以左下角为原点的标准xy坐标系
 *
 */


const Smt_map_t map_soduko[4][5] = {

    // 第一列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 0},     // 0 0
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 0 1
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 0 2
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1},     // 0 3
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },

    // 第二列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0},     // 1 0
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1},     // 1 1
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1},     // 1 2
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1},     // 1 3
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },

    // 第三列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0},     // 2 0
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1},     // 2 1
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1},     // 2 2
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1},     // 2 3
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },

    // 第四列
    {
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 0}, // 3 0
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1}, // 3 1
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1}, // 3 1/5
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1}, // 3 2
        {.x_fr = 0, .y_fr = 0, .x_br = 1, .y_br = 1}, // 3 3
    },

};

const Smt_map_t map_transport[7][12] = {
    // 第1列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 1 0//如何处理
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1},     // 1 1
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 0},     // 1 2
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1},     // 1 3
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },                                                    // 第1列
                                                          // 第2列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 0},     // 2 0
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 2 1
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 2 2
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1},     // 2 3
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 0},     // 2 4
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1},     // 2 5
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 0},     // 2 6
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 2 7
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 2 8
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1},     // 2 9
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },                                                    // 第2列
                                                          // 第3列
    {
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 0},     // 3 0
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 0},     // 3 1
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0},     // 3 2
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1},     // 3 3
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1},     // 3 4
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1},     // 3 5
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 0},     // 3 6
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },                                                    // 第3列
    // 第4列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1}, // 4 0
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1}, // 4 1
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1}, // 4 2
        {.x_fr = 0, .y_fr = 0, .x_br = 1, .y_br = 1}, // 4 3
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 0}, // 4 4
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1}, // 4 5
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1}, // 4 6
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1}, // 4 7
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 0}, // 4 8
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1}, // 4 9
        {.x_fr = 0, .y_fr = 1, .x_br = 0, .y_br = 1}, // 4 10
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1}, // 4 11
    },                                                // 第4列
        // 第5列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0}, // 5 0
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1}, // 5 1
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 0}, // 5 2
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 0}, // 5 3
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 0}, // 5 4
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1}, // 5 5
        {.x_fr = 0, .y_fr = 0, .x_br = 1, .y_br = 1}, // 5 6
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0}, // 5 7
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1}, // 5 8
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },                                                // 第5列
        // 第6列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0}, // 6 0
        {.x_fr = 1, .y_fr = 0, .x_br = 0, .y_br = 1}, // 6 1
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 0}, // 6 2
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1}, // 6 3
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 1}, // 6 4
        {.x_fr = 0, .y_fr = 0, .x_br = 1, .y_br = 1}, // 6 5
        {.x_fr = 1, .y_fr = 1, .x_br = 0, .y_br = 0}, // 6 6

        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1}, // 6 7
        {.x_fr = 0, .y_fr = 0, .x_br = 1, .y_br = 1}, // 6 8
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1}, // 6 9
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },                                                // 第6列
    // 第7列
    {
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 1},     // 7 0
        {.x_fr = 1, .y_fr = 0, .x_br = 1, .y_br = 1},     // 7 1
        {.x_fr = 1, .y_fr = 1, .x_br = 1, .y_br = 0},     // 7 2
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1},     // 7 3
        {.x_fr = 0, .y_fr = 1, .x_br = 1, .y_br = 1},     // 7 4
        {.x_fr = 0, .y_fr = 0, .x_br = 1, .y_br = 1},     // 7 4
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
        {.x_fr = -1, .y_fr = -1, .x_br = -1, .y_br = -1}, // nouse
    },                                                    // 第7列

};

// 将一个地图元素打印出来,元素要占据两行，对于一个四个方向都支持的地图元素，打印出来的样子如下
//   ↑
// ←   →
//   ↓
// 对于一个只支持左右方向的地图元素，打印出来的样子如下
//   x
// ←   →
//   x
// 打印可以使用控制字符实现
void printf_map_element(const Smt_map_t *map_element, int n)
{
    // 创建一个地图元素的字符串
    char map_element_str[3][16] = {
        "   x   \r\n",
        "x     x \r\n",
        "   x   \r\n",
    };
    // 根据地图元素的方向，修改地图元素的字符串
    if (map_element->y_fr == 1)
    {
        map_element_str[0][3] = '|';
    }
    else if (map_element->y_fr == -1)
    {
        map_element_str[0][3] = ' ';
    }
    if (map_element->x_fr == 1)
    {
        map_element_str[1][5] = '-';
        map_element_str[1][6] = '-';
    }
    else if (map_element->x_fr == -1)
    {
        map_element_str[1][5] = ' ';
        map_element_str[1][6] = ' ';
    }

    if (map_element->y_br == 1)
    {
        map_element_str[2][3] = '|';
    }
    else if (map_element->y_br == -1)
    {
        map_element_str[2][3] = ' ';
    }
    if (map_element->x_br == 1)
    {
        map_element_str[1][0] = '-';
        map_element_str[1][1] = '-';
    }
    else if (map_element->x_br == -1)
    {
        map_element_str[1][0] = ' ';
        map_element_str[1][1] = ' ';
    }

    for (int i = 0; i < n; ++i)
    {
        printf("\t");
    }
    // 打印地图元素的字符串
    printf("%s", map_element_str[0]);
    for (int i = 0; i < n; ++i)
    {
        printf("\t");
    }
    printf("%s", map_element_str[1]);
    for (int i = 0; i < n; ++i)
    {
        printf("\t");
    }
    printf("%s", map_element_str[2]);

    // 使用控制字符回退到上面三行
    printf("\033[4A");
    printf("\n");

}

void print_map_soduko(void)
{
    printf("MAP Soduko:\r\n");

    for (int j = 4; j > -1; j--)
    {
        for (int i = 0; i < 4; i++)
        {
            printf_map_element(&map_soduko[i][j], i);
        }
        printf("\r\n\n\n\n\n");
    }
}

void printf_map_transport(void)
{
    printf("MAP Transport:\r\n");
    for (int j = 11; j > -1; j--)
    {
        for (int i = 0; i < 7; i++)
        {
            printf_map_element(&map_transport[i][j], i);
        }
        printf("\r\n\n\n\n\n");
    }
}




















