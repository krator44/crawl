/*
 * Copyright 2021 krator44
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

class MersenneTwister {
  public:
  static const u_int32_t tx_w = 32;
  static const u_int32_t tx_n = 624;
  static const u_int32_t tx_m = 397;
  //static const u_int32_t tx_r = 31;

  static const u_int32_t tx_a = 0x9908b0df;

  static const u_int32_t tx_u = 11;
  static const u_int32_t tx_d = 0xffffffff;

  static const u_int32_t tx_s = 7;
  static const u_int32_t tx_b = 0x9d2c5680;

  static const u_int32_t tx_t = 15;
  static const u_int32_t tx_c = 0xefc60000;

  static const u_int32_t tx_l = 18;

  static const u_int32_t tx_f = 1812433253;

  static const u_int32_t tx_lower = 0x7fffffff;
  static const u_int32_t tx_upper = 0x80000000;

  static const u_int32_t tx_fs = 19650218;
  static const u_int32_t tx_f1 = 1664525;
  static const u_int32_t tx_f2 = 1566083941;

  public:
  u_int32_t rt[tx_n];
  u_int32_t tt;
  MersenneTwister() {
    for(u_int32_t i=0; i < tx_n; i++) {
      rt[i] = 0;
    }
    tt = tx_n + 1;

    // initialize arbitrarily
    init(0xd2a72df0);
  }

  void save(const char *fn) {
    FILE *ff;
    u_int32_t i, kk;
    if (tt > tx_n) {
      printf("error in mersenne.h\n");
      printf("generator not seeded\n");
      exit(0);
    }
    ff = fopen(fn, "w");
    if (ff == 0) {
      return;
    }
    fprintf (ff, "%u\n", tt);
    kk = 0;
    for (i = 0; i < tx_n; i++) {
      fprintf(ff, "%08x ", rt[i]);
      kk++;
      if (kk == 4) {
        fprintf(ff, "\n");
        kk = 0;
      }
    }
    fprintf(ff, "\n");
    fclose(ff);
  }

  int restore(const char *fn) {
    FILE *ff;
    u_int32_t i;
    int ok;
    ff = fopen(fn, "r");
    if (ff == 0) {
      return 1;
    }
    ok = fscanf(ff, "%u\n", &tt);
    if (ok != 1) {
      fclose(ff);
      return 1;
    }
    for (i = 0; i < tx_n; i++) {
      ok = fscanf(ff, "%08x ", &rt[i]);
      if (ok != 1) {
        fclose(ff);
        return 1;
      }
    }
    fclose(ff);
    return 0;
  }

  void init(u_int32_t seed) {
    rt[0] = seed;
    u_int32_t xx;
    for(u_int32_t i = 1; i < tx_n; i++) {
      xx = rt[i-1];
      xx = xx ^ (rt[i-1] >> (tx_w-2));
      xx = xx * tx_f;
      xx = xx + i;
      rt[i] = xx;
    }
    tt = tx_n;
  }

  void disturb(u_int32_t seed) {
    rt[0] ^= seed;
    u_int32_t xx;
    for(u_int32_t i = 1; i < tx_n; i++) {
      xx = rt[i-1];
      xx = xx ^ (rt[i-1] >> (tx_w-2));
      xx = xx * tx_f;
      xx = xx + i;
      rt[i] ^= xx;
    }
  }

  void init_array(u_int32_t *seed, u_int32_t length) {
    init(tx_fs);

    u_int32_t n;
    if (tx_n > length) {
      n = tx_n;
    }
    else {
      n = length;
    }

    u_int32_t xx;

    u_int32_t i=1;
    u_int32_t j=0;

    for (u_int32_t k=0; k < n; k++) {
      xx = rt[i-1] >> 30;
      xx = xx ^ rt[i-1];
      xx = xx * tx_f1;
      xx = xx ^ rt[i];
      xx = xx + seed[j];
      xx = xx + j;
      rt[i] = xx;
      i++;
      j++;
      if (i >= tx_n) {
        rt[0] = rt[tx_n - 1];
        i = 1;
      }
      if (j >= length) {
        j = 0;
      }
    }

    for (u_int32_t k=0; k < tx_n - 1; k++) {
      xx = rt[i-1] >> 30;
      xx = xx ^ rt[i-1];
      xx = xx * tx_f2;
      xx = xx ^ rt[i];
      xx = xx - i;
      rt[i] = xx;
      i++;
      if (i >= tx_n) {
        rt[0] = rt[tx_n - 1];
        i = 1;
      }
    }
    rt[0] = 0x80000000;
  }
  
  void charge(MersenneTwister *mt) {
    u_int32_t seed[tx_n];
    for(u_int32_t i=0; i < tx_n; i++) {
      seed[i] = mt->rand32();
    }
    init_array(seed, tx_n);
  }

  u_int32_t rand32() {
    u_int32_t xx;
    if(tt > tx_n) {
      printf("error in mersenne.h\n");
      printf("generator not seeded\n");
      exit(0);
    }
    else if (tt == tx_n) {
      shuffle();
    }
    xx = temper(rt[tt]);
    tt++;
    return xx;
  }

  u_int32_t temper(u_int32_t x) {
    x = x ^ ((x >> tx_u) & tx_d);
    x = x ^ ((x << tx_s) & tx_b);
    x = x ^ ((x << tx_t) & tx_c);
    x = x ^ (x >> tx_l);
    return x;
  }

  double real() {
    double fx;
    fx = (double) rand32();
    fx = fx / 4294967296.0;
    return fx;
  }

  void shuffle() {
    u_int32_t upper, lower;
    u_int32_t x;
    u_int32_t next;
    for(u_int32_t i=0; i < tx_n; i++) {
      next = (i+1) % tx_n;
      upper = rt[i] & tx_upper;
      lower = rt[next] & tx_lower;
      x = upper | lower;
      if(x % 2 == 0) {
        x = (x >> 1);
      }
      else {
        x = (x >> 1) ^ tx_a;
      }
      next = (i + tx_m) % tx_n;
      rt[i] = rt[next] ^ x;
    }
    tt = 0;
  }

};


