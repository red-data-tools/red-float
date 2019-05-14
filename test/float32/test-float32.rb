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

class Float32Test < Test::Unit::TestCase
  test("superclass") do
    assert_equal(Numeric,
                 Float32.superclass)
  end

  test(".allocate") do
    assert_raise_kind_of(TypeError) do
      Float32.allocate
    end
  end

  test(".new") do
    assert_raise_kind_of(NoMethodError) do
      Float32.new
    end
  end

  test("Float32()") do
    assert_kind_of(Float32,
                   Float32(0))
    assert_kind_of(Float32,
                   Float32(1<<512 - 1))
    assert_kind_of(Float32,
                   Float32(0.0))
    assert_kind_of(Float32,
                   Float32(0/1r))
    assert_raise_kind_of(RangeError) do
      Float32(1 + 2i)
    end
    assert_raise_kind_of(TypeError) do
      Float32(Object.new)
    end
    assert_kind_of(Float32,
                   Float32('0.0'))

    class <<(obj_with_to_f = Object.new)
      def to_f
        42.0
      end
    end
    assert_raise_kind_of(TypeError) do
      Float32(obj_with_to_f)
    end

    assert_equal(Float32(0), Float32(0.0))
  end

  test("#-@") do
    assert_equal(Float32(-1),
                 -Float32(1))
  end

  test("#+") do
    assert_equal(Float32(3),
                 Float32(1) + Float32(2))
  end

  test("#-") do
    assert_equal(Float32(-1),
                 Float32(1) - Float32(2))
  end

  test("#*") do
    assert_equal(Float32(6),
                 Float32(2) * Float32(3))
  end

  test("#/") do
    assert(Float32(0.5),
           Float32(1) / Float32(2))
  end

  test("#==") do
    assert_operator(Float32(0), :==,0)
    assert_not_operator(Float32(1), :==, Float32(2))
  end

  test("#!=") do
    assert_not_operator(Float32(0), :!=,0)
    assert_operator(Float32(1), :!=, Float32(2))
  end

  test("#<") do
    assert_operator(Float32(1), :<, Float32(2))
  end

  test("#<=") do
    assert_operator(Float32(1), :<=, Float32(2))
  end

  test("#>") do
    assert_not_operator(Float32(1), :>, Float32(2))
  end

  test("#>=") do
    assert_not_operator(Float32(1), :>=, Float32(2))
  end

  test("#nan?") do
    assert_predicate(Float32::NAN, :nan?)
    assert_not_predicate(Float32(0), :nan?)
    assert_not_predicate(Float32::INFINITY, :nan?)
  end

  test("#next_float") do
    assert_equal(Float32::EPSILON,
                 Float32(1).next_float - Float32(1))
    assert_equal(Float32::EPSILON / 2,
                 Float32(-1).next_float + Float32(1))

    smallest = Float32(0).next_float
    assert_operator(Float32(0), :<, smallest)
    assert_operator([Float32(0), smallest], :include?, smallest/2)

    assert_equal(Float32::INFINITY,
                 Float32::MAX.next_float)
    assert_equal(Float32::INFINITY,
                 Float32::INFINITY.next_float)
    assert_equal(-Float32::MAX,
                 (-Float32::INFINITY).next_float)

    assert_predicate(Float32::NAN.next_float, :nan?)
  end
end
