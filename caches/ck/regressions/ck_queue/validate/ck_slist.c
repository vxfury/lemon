/*
 * Copyright 2012-2015 Samy Al Bahra.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <ck_queue.h>

#include "../../common.h"

struct test {
    int value;
    CK_SLIST_ENTRY(test) list_entry;
};
static CK_SLIST_HEAD(test_list, test) head = CK_SLIST_HEAD_INITIALIZER(head);

static int goal;

static void test_foreach(void)
{
    struct test *n, *next, *safe;
    int i, s = 0, j = 0, k = 0;

    for (i = goal; i != 0; i = goal) {
        s = 0;

        CK_SLIST_FOREACH(n, &head, list_entry)
        {
            j++;
            if (s == 0)
                s = n->value;
            else
                s = s - 1;

            if (n->value != s) {
                ck_error("\nExpected %d, but got %d.\n", s, n->value);
            }

            next = CK_SLIST_NEXT(n, list_entry);
            if (next != NULL && next->value != s - 1) {
                ck_error("\nExpected %d, but got %d.\n", s, next->value);
            }

            i--;
        }

        if (i == 0) break;

        s = 0;
        CK_SLIST_FOREACH_SAFE(n, &head, list_entry, safe)
        {
            k++;

            if (s == 0)
                s = n->value;
            else
                s = s - 1;

            if (n->value != s) {
                ck_error("\nExpected %d, but got %d.\n", s, n->value);
            }

            next = CK_SLIST_NEXT(n, list_entry);
            if (next != NULL && next->value != s - 1) {
                ck_error("\nExpected %d, but got %d.\n", s, next->value);
            }

            i--;
        }

        if (i == 0 || CK_SLIST_EMPTY(&head) == true) break;
    }

    fprintf(stderr, "(%d, %d) ", j, k);
    return;
}

static void *execute(void *c)
{
    (void)c;
    test_foreach();
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t *thread;
    struct test *n;
    struct test_list target;
    int n_threads, i;

    if (argc != 3) {
        ck_error("Usage: %s <number of threads> <number of list entries>\n", argv[0]);
    }

    n_threads = atoi(argv[1]);
    if (n_threads < 1) {
        ck_error("ERROR: Number of threads must be >= 1.\n");
    }

    thread = malloc(sizeof(pthread_t) * n_threads);
    assert(thread != NULL);

    goal = atoi(argv[2]);
    if (goal < 4) {
        ck_error("ERROR: Number of entries must be >= 4.\n");
    }

    fprintf(stderr, "Beginning serial test...");
    CK_SLIST_INIT(&head);

    for (i = 1; i <= goal; i++) {
        n = malloc(sizeof *n);
        assert(n != NULL);
        n->value = i;
        CK_SLIST_INSERT_HEAD(&head, n, list_entry);
    }

    test_foreach();

    for (i = 1; i <= goal; i++) {
        n = CK_SLIST_FIRST(&head);
        CK_SLIST_REMOVE_HEAD(&head, list_entry);
        free(n);
    }

    if (CK_SLIST_EMPTY(&head) == false) {
        ck_error("List is not empty after bulk removal.\n");
    }

    fprintf(stderr, "done (success)\n");

    fprintf(stderr, "Beginning parallel traversal...");

    n = malloc(sizeof *n);
    assert(n != NULL);
    n->value = 1;
    CK_SLIST_INSERT_HEAD(&head, n, list_entry);

    for (i = 0; i < n_threads; i++) {
        int r = pthread_create(&thread[i], NULL, execute, NULL);
        assert(r == 0);
    }

    for (i = 2; i <= goal; i++) {
        volatile int j;

        n = malloc(sizeof *n);
        assert(n != NULL);
        n->value = i;
        CK_SLIST_INSERT_HEAD(&head, n, list_entry);
        for (j = 0; j <= 1000; j++)
            ;
    }

    for (i = 0; i < n_threads; i++) pthread_join(thread[i], NULL);

    for (i = 0; i < n_threads; i++) {
        int r = pthread_create(&thread[i], NULL, execute, NULL);
        assert(r == 0);
    }

    CK_SLIST_MOVE(&target, &head, list_entry);

    for (i = 1; i <= goal; i++) {
        volatile int j;

        if (CK_SLIST_EMPTY(&target) == false) CK_SLIST_REMOVE_HEAD(&target, list_entry);

        for (j = 0; j <= 1000; j++)
            ;

        if (CK_SLIST_EMPTY(&target) == false) {
            struct test *r = CK_SLIST_FIRST(&target);
            CK_SLIST_REMOVE(&target, r, test, list_entry);
        }
    }

    for (i = 0; i < n_threads; i++) pthread_join(thread[i], NULL);

    fprintf(stderr, "done (success)\n");
    return (0);
}
