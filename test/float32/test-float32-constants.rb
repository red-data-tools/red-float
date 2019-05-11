# frozen_string_literal: true
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

class Float32ConstantsTest < Test::Unit::TestCase
  test("MANT_DIG") do
    assert_kind_of(Integer, Float32::MANT_DIG)
  end

  test("DIG") do
    assert_kind_of(Integer, Float32::DIG)
  end

  test("MIN_EXP") do
    assert_kind_of(Integer, Float32::MIN_EXP)
  end

  test("MAX_EXP") do
    assert_kind_of(Integer, Float32::MAX_EXP)
  end

  test("MIN_10_EXP") do
    assert_kind_of(Integer, Float32::MIN_10_EXP)
  end

  test("MAX_10_EXP") do
    assert_kind_of(Integer, Float32::MAX_10_EXP)
  end

  test("MIN") do
    assert_kind_of(Float32, Float32::MIN)
  end

  test("MAX") do
    assert_kind_of(Float32, Float32::MAX)
  end

  test("INFINITY") do
    assert_kind_of(Float32, Float32::INFINITY)
  end

  test("NAN") do
    assert_kind_of(Float32, Float32::NAN)
  end
end
