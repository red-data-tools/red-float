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
end
