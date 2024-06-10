#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NUM_FAT_BLOCK 1024

typedef struct {
  int next;
} Fat;

Fat fat[NUM_FAT_BLOCK];

int num_free_block;

// ブロックが空かどうか判定する
bool is_free(int block_index) { return fat[block_index].next == -2; }

// next: 次のブロック番号
// FATのすべてのブロックのnextを-2にする
void fat_clear() {
  for (int i = 0; i < NUM_FAT_BLOCK; i++) {
    // 0以上はブロック番号であり、-1は連結リストの終わりを表す番号なので-2で初期化する
    fat[i].next = -2;
  }
  num_free_block = NUM_FAT_BLOCK;
}

// 必要なブロックを割り当てる
// 割り当てに成功した場合: 割り当てた領域の先頭のブロック番号を返す
// 割り当てに失敗した場合: -1を返す
int fat_allocate(int num_block_required) {
  int head;
  int count_free_block = 0;
  int prev_index;

  for (int i = 0; i < NUM_FAT_BLOCK; i++) {
    if (is_free(i)) {
      head = i;
      break;
    }
  }

  for (int i = 0; i < NUM_FAT_BLOCK; i++) {
    if (is_free(i)) {
      count_free_block++;
    }
  }

  if (num_block_required > count_free_block) {
    printf("ブロック割り当てに失敗しました\n");
    return -1;
  } else {
    prev_index = head;

    for (int i = head; i < NUM_FAT_BLOCK; i++) {
      if (is_free(i)) {
        if (fat[prev_index].next == -1) {
          fat[prev_index].next = i;
        }
        fat[i].next = -1;
        num_free_block--;
        num_block_required--;
        prev_index = i;
        if (num_block_required == 0) {
          break;
        }
      }
    }
    return head;
  }
}
// ブロック割り当てが適切かどうかチェックする
void fat_verify() {
  int num_block = 0;
  for (int i = 0; i < NUM_FAT_BLOCK; i++) {
    if (!is_free(i)) {
      num_block++;
    }
  }
  if (num_block == (NUM_FAT_BLOCK - num_free_block)) {
    printf("ブロック割り当ては適切です\n");
  } else {
    printf("ブロック割り当てが不適切です\n");
  }
  printf("空きブロック数: %d, 割り当て済みのブロック数: %d\n", num_free_block,
         num_block);
}

// ブロックを解放する
// 解放できた場合: 0を返す
// 解放できなかった場合: -1を返す
int fat_free(int block_index, int num_block) {
  if (block_index < 0 || block_index > NUM_FAT_BLOCK - 1) {
    printf("管理していないブロックは開放できません\n");
    return -1;
  } else {
    for (int i = 0; i < num_block; i++) {
      if (!is_free(block_index + i)) {
        fat[block_index].next = -2;
        num_free_block++;
      }
    }
    return 0;
  }
}

void fat_list(int block_index) {
  int current_index = block_index;
  while (fat[current_index].next != -2) {
    printf("%d", current_index);
    if (fat[current_index].next == -1) {
      printf("\n");
      break;
    } else {
      printf(" => ");
      current_index = fat[current_index].next;
    }
  }
}

// データの状態を出力する
void fat_dump() {
  for (int i = 0; i < NUM_FAT_BLOCK; i++) {
    putchar(is_free(i) ? '0' : '1');
    if (i % 64 == 63) {
      putchar('\n');
    } else if (i % 8 == 7) {
      putchar(' ');
    }
  }
}

int main() {
  fat_clear();

  // 正常系テスト
  // fat_allocate(2);
  // fat_free(0, 1);
  // fat_allocate(10);
  // fat_list(0);

  // 異常系テスト
  // fat_allocate(1025);
  // fat_free(-1, 1);

  fat_dump();
  fat_verify();
  return 0;
}
