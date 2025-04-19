#pragma once

enum {
  CONNECT = 1,
  EVOLVE,
  ATTACK,
  REINSTALL,
  FUSION,
  STATUS,
  REVERT
};

struct Query {
  int type;
  union {
    struct {
      int x, y;
    } connect;
    struct {
      int t;
    } evolve, attack;
    struct {
      int k, s;
    } reinstall;
    struct {
      int a, b;
    } fusion;
    struct {
      int k;
    } status;
    struct {
    } revert;
  };
};