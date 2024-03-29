// ----------------------------------------------------------------------------
// VECTOR OBJECT UNIT TESTS
//
// Code By: Michael Wrona
// Created: 23 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for vector objects.
 */


#ifdef UNIT_TEST
#pragma once

#include <unity.h>
#include <Arduino.h>
#include "maths/vectors.h"

void test_vectorf_default_len(void);
void test_vectord_default_len(void);
void test_vectorf_default_vals(void);
void test_vectord_default_vals(void);
void test_vectorf_norm(void);
void test_vectord_norm(void);

#endif
