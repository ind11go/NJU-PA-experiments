/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[256];
  word_t old_val; 

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int next_no = 1;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){
  if (free_ == NULL){
    printf("Error: No free watchpoints available!\n");
    assert(0);
  }
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  wp->NO = next_no++;
  wp->expr[0] = '\0';
  wp->old_val = 0;
  return wp;
}
void free_wp(WP *wp) {
  if (wp == NULL || head == NULL) {
    printf("Error: Try to free an invalid watchpoint!\n");
    assert(0);
  }

  if (head == wp) {
    head = head->next;
  }
  else {
    WP *prev = head; 
    while (prev->next != NULL && prev->next != wp) {
      prev = prev->next;
    }
    if (prev->next == NULL) {
      printf("Error: Watchpoint not found in the list!\n");
      assert(0); 
    }
    prev->next = wp->next;
  }

  wp->next = free_;
  free_ = wp;

  wp->old_val = 0;
  memset(wp->expr, 0, sizeof(wp->expr));
}
void info_wp(){
  if (head == NULL){
    printf("No watchpoints.\n");
    return;
  }
  printf("%-8s\t%-16s\t%-16s\n", "Num", "Type", "What");
  WP *p = head;
  while (p != NULL){
    printf("%-8d\t%-16s\t%-16s\n", p->NO, "watchpoint", p->expr);
    p = p->next;
  }
}
void add_watchpoint(char *args){
  WP *wp = new_wp();
  strcpy(wp->expr, args);
  bool success;
  wp->old_val = expr(args, &success);
  if(!success){
    printf("表达式求值失败！\n");
    free_wp(wp);
    return;
  }
  printf("成功设置监视点 %d: %s\n", wp->NO, wp->expr);
}
void delete_watchpoint(int no){
  WP *p = head;
  
  while (p != NULL) {
    if (p->NO == no) {
      free_wp(p);
      printf("Watchpoint %d deleted\n", no);
      return; 
    }
    p = p->next;
  }
  printf("Watchpoint %d not found. Please check your watchpoint number using 'info w'.\n", no);


}
bool check_watchpoint() {
  if (head == NULL) {
    return false; 
  }

  WP *p = head;
  bool is_changed = false;

  while (p != NULL) {
    bool success = false;
    word_t new_val = expr(p->expr, &success);

    if (!success) {
      printf("Error: Failed to evaluate expression '%s' during execution.\n", p->expr);
      assert(0);
    }

    if (new_val != p->old_val) {
      printf("\nHit watchpoint %d: %s\n", p->NO, p->expr);
      printf("Old value = %u (0x%08x)\n", p->old_val, p->old_val);
      printf("New value = %u (0x%08x)\n", new_val, new_val);

      p->old_val = new_val;
      is_changed = true;
    }

    p = p->next;
  }

  return is_changed; 
}
