# -*- ruby -*-
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

require_relative "lib/red-float/version"

Gem::Specification.new do |spec|
  spec.name = "red-float"
  version_components = [
    RedFloat::Version::MAJOR.to_s,
    RedFloat::Version::MINOR.to_s,
    RedFloat::Version::MICRO.to_s,
    RedFloat::Version::TAG,
  ]
  spec.version = version_components.compact.join(".")
  spec.homepage = "https://github.com/red-data-tools/red-float"
  spec.authors = ["Kenta Murata"]
  spec.email = ["mrkn@mrkn.jp"]

  spec.summary = "Float32 and Float16 for Ruby"
  spec.description = "red-float provides Float32 and Float16 classes for Ruby"
  spec.license = "Apache-2.0"

  spec.files = ["README.md", "Rakefile", "Gemfile", "#{spec.name}.gemspec"]
  spec.files += ["LICENSE.txt"]
  spec.files += Dir.glob("ext/**/*.{c,h,rb}")
  spec.files += Dir.glob("lib/**/*.rb")

  spec.test_files += Dir.glob("test/**/*")

  spec.extensions = ["ext/float32/extconf.rb", "ext/float16/extconf.rb"]

  spec.add_runtime_dependency("extpp")
  spec.add_runtime_dependency("native-package-installer")
  spec.add_runtime_dependency("pkg-config")

  spec.add_development_dependency("bundler")
  spec.add_development_dependency("rake")
  spec.add_development_dependency("test-unit")
end
