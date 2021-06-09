// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: MBVTPackage.proto

#ifndef PROTOBUF_MBVTPackage_2eproto__INCLUDED
#define PROTOBUF_MBVTPackage_2eproto__INCLUDED

#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>
#include "protobuf/pbf.hpp"

// @@protoc_insertion_point(includes)

namespace vector_tile {
class Tile;
class Tile_Value;
class Tile_Feature;
class Tile_Layer;

enum Tile_GeomType {
  Tile_GeomType_UNKNOWN = 0,
  Tile_GeomType_POINT = 1,
  Tile_GeomType_LINESTRING = 2,
  Tile_GeomType_POLYGON = 3
};
// ===================================================================

class Tile_Value {
public:
  inline Tile_Value();
  inline explicit Tile_Value(const protobuf::message& srcMsg);
  #if _PROTOBUF_USE_RVALUE_REFS
  inline Tile_Value(const Tile_Value&) = default;
  inline Tile_Value(Tile_Value&&) = default;
  inline Tile_Value& operator = (const Tile_Value&) = default;
  inline Tile_Value& operator = (Tile_Value&&) = default;
  #endif

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string string_value = 1;
  static const int kStringValueFieldNumber = 1;
  inline bool has_string_value() const;
  inline const ::std::string& string_value() const;

  // optional float float_value = 2;
  static const int kFloatValueFieldNumber = 2;
  inline bool has_float_value() const;
  inline float float_value() const;

  // optional double double_value = 3;
  static const int kDoubleValueFieldNumber = 3;
  inline bool has_double_value() const;
  inline double double_value() const;

  // optional int64 int_value = 4;
  static const int kIntValueFieldNumber = 4;
  inline bool has_int_value() const;
  inline std::int64_t int_value() const;

  // optional uint64 uint_value = 5;
  static const int kUintValueFieldNumber = 5;
  inline bool has_uint_value() const;
  inline std::uint64_t uint_value() const;

  // optional sint64 sint_value = 6;
  static const int kSintValueFieldNumber = 6;
  inline bool has_sint_value() const;
  inline std::int64_t sint_value() const;

  // optional bool bool_value = 7;
  static const int kBoolValueFieldNumber = 7;
  inline bool has_bool_value() const;
  inline bool bool_value() const;

  // @@protoc_insertion_point(class_scope:vector_tile.Tile.Value)
private:
  std::uint32_t _has_bits_[1];
  ::std::string string_value_ = "";
  double double_value_ = 0;
  std::int64_t int_value_ = 0ll;
  float float_value_ = 0;
  bool bool_value_ = false;
  std::uint64_t uint_value_ = 0ull;
  std::int64_t sint_value_ = 0ll;
};
// -------------------------------------------------------------------

class Tile_Feature {
public:
  inline Tile_Feature();
  inline explicit Tile_Feature(const protobuf::message& srcMsg);
  #if _PROTOBUF_USE_RVALUE_REFS
  inline Tile_Feature(const Tile_Feature&) = default;
  inline Tile_Feature(Tile_Feature&&) = default;
  inline Tile_Feature& operator = (const Tile_Feature&) = default;
  inline Tile_Feature& operator = (Tile_Feature&&) = default;
  #endif

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint64 id = 1 [default = 0];
  static const int kIdFieldNumber = 1;
  inline bool has_id() const;
  inline std::uint64_t id() const;

  // repeated uint32 tags = 2 [packed = true];
  static const int kTagsFieldNumber = 2;
  inline int tags_size() const;
  inline std::uint32_t tags(int index) const;
  inline const std::vector< std::uint32_t >& tags() const;

  // optional .vector_tile.Tile.GeomType type = 3 [default = UNKNOWN];
  static const int kTypeFieldNumber = 3;
  inline bool has_type() const;
  inline ::vector_tile::Tile_GeomType type() const;

  // repeated uint32 geometry = 4 [packed = true];
  static const int kGeometryFieldNumber = 4;
  inline int geometry_size() const;
  inline std::uint32_t geometry(int index) const;
  inline const std::vector< std::uint32_t >& geometry() const;

  // @@protoc_insertion_point(class_scope:vector_tile.Tile.Feature)
  void set_id(std::int64_t id) { id_ = id; }
private:
  std::uint32_t _has_bits_[1];
  std::uint64_t id_ = 0ull;
  std::vector< std::uint32_t > tags_;
  std::vector< std::uint32_t > geometry_;
  int type_ = 0;
};
// -------------------------------------------------------------------

class Tile_Layer {
public:
  inline Tile_Layer();
  inline explicit Tile_Layer(const protobuf::message& srcMsg);
  #if _PROTOBUF_USE_RVALUE_REFS
  inline Tile_Layer(const Tile_Layer&) = default;
  inline Tile_Layer(Tile_Layer&&) = default;
  inline Tile_Layer& operator = (const Tile_Layer&) = default;
  inline Tile_Layer& operator = (Tile_Layer&&) = default;
  #endif

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 version = 15 [default = 1];
  static const int kVersionFieldNumber = 15;
  inline bool has_version() const;
  inline std::uint32_t version() const;

  // required string name = 1;
  static const int kNameFieldNumber = 1;
  inline bool has_name() const;
  inline const ::std::string& name() const;

  // repeated .vector_tile.Tile.Feature features = 2;
  static const int kFeaturesFieldNumber = 2;
  inline int features_size() const;
  inline const ::vector_tile::Tile_Feature& features(int index) const;
  inline const std::vector< ::vector_tile::Tile_Feature >& features() const;

  // repeated string keys = 3;
  static const int kKeysFieldNumber = 3;
  inline int keys_size() const;
  inline const ::std::string& keys(int index) const;
  inline const std::vector< ::std::string >& keys() const;

  // repeated .vector_tile.Tile.Value values = 4;
  static const int kValuesFieldNumber = 4;
  inline int values_size() const;
  inline const ::vector_tile::Tile_Value& values(int index) const;
  inline const std::vector< ::vector_tile::Tile_Value >& values() const;

  // optional uint32 extent = 5 [default = 4096];
  static const int kExtentFieldNumber = 5;
  inline bool has_extent() const;
  inline std::uint32_t extent() const;

  // @@protoc_insertion_point(class_scope:vector_tile.Tile.Layer)
private:
  std::uint32_t _has_bits_[1];
  ::std::string name_ = "";
  std::vector< ::vector_tile::Tile_Feature > features_;
  std::uint32_t version_ = 1u;
  std::uint32_t extent_ = 4096u;
  std::vector< ::std::string > keys_;
  std::vector< ::vector_tile::Tile_Value > values_;
};
// -------------------------------------------------------------------

class Tile {
public:
  inline Tile();
  inline explicit Tile(const protobuf::message& srcMsg);
  #if _PROTOBUF_USE_RVALUE_REFS
  inline Tile(const Tile&) = default;
  inline Tile(Tile&&) = default;
  inline Tile& operator = (const Tile&) = default;
  inline Tile& operator = (Tile&&) = default;
  #endif

  // nested types ----------------------------------------------------

  typedef Tile_Value Value;
  typedef Tile_Feature Feature;
  typedef Tile_Layer Layer;

  typedef Tile_GeomType GeomType;
  static const GeomType UNKNOWN = Tile_GeomType_UNKNOWN;
  static const GeomType POINT = Tile_GeomType_POINT;
  static const GeomType LINESTRING = Tile_GeomType_LINESTRING;
  static const GeomType POLYGON = Tile_GeomType_POLYGON;

  // accessors -------------------------------------------------------

  // repeated .vector_tile.Tile.Layer layers = 3;
  static const int kLayersFieldNumber = 3;
  inline int layers_size() const;
  inline const ::vector_tile::Tile_Layer& layers(int index) const;
  inline const std::vector< ::vector_tile::Tile_Layer >& layers() const;

  // @@protoc_insertion_point(class_scope:vector_tile.Tile)
private:
  std::uint32_t _has_bits_[1];
  std::vector< ::vector_tile::Tile_Layer > layers_;
};
// ===================================================================

// Tile_Value

inline Tile_Value::Tile_Value() {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
}

inline Tile_Value::Tile_Value(const protobuf::message& srcMsg) {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
  for (protobuf::message msg(srcMsg); msg.next(); ) {
    if (msg.tag == kStringValueFieldNumber) {
      string_value_ = msg.read_string();
      _has_bits_[0] |= 0x00000001u;
    }
    else if (msg.tag == kFloatValueFieldNumber) {
      float_value_ = msg.read_float();
      _has_bits_[0] |= 0x00000002u;
    }
    else if (msg.tag == kDoubleValueFieldNumber) {
      double_value_ = msg.read_double();
      _has_bits_[0] |= 0x00000004u;
    }
    else if (msg.tag == kIntValueFieldNumber) {
      int_value_ = msg.read_int64();
      _has_bits_[0] |= 0x00000008u;
    }
    else if (msg.tag == kUintValueFieldNumber) {
      uint_value_ = msg.read_uint64();
      _has_bits_[0] |= 0x00000010u;
    }
    else if (msg.tag == kSintValueFieldNumber) {
      sint_value_ = msg.read_sint64();
      _has_bits_[0] |= 0x00000020u;
    }
    else if (msg.tag == kBoolValueFieldNumber) {
      bool_value_ = msg.read_bool();
      _has_bits_[0] |= 0x00000040u;
    }
    else msg.skip();
  }
}

// optional string string_value = 1;
inline bool Tile_Value::has_string_value() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}

inline const ::std::string& Tile_Value::string_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.string_value)
  return string_value_;
}

// optional float float_value = 2;
inline bool Tile_Value::has_float_value() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}

inline float Tile_Value::float_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.float_value)
  return float_value_;
}

// optional double double_value = 3;
inline bool Tile_Value::has_double_value() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}

inline double Tile_Value::double_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.double_value)
  return double_value_;
}

// optional int64 int_value = 4;
inline bool Tile_Value::has_int_value() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}

inline std::int64_t Tile_Value::int_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.int_value)
  return int_value_;
}

// optional uint64 uint_value = 5;
inline bool Tile_Value::has_uint_value() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}

inline std::uint64_t Tile_Value::uint_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.uint_value)
  return uint_value_;
}

// optional sint64 sint_value = 6;
inline bool Tile_Value::has_sint_value() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}

inline std::int64_t Tile_Value::sint_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.sint_value)
  return sint_value_;
}

// optional bool bool_value = 7;
inline bool Tile_Value::has_bool_value() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}

inline bool Tile_Value::bool_value() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Value.bool_value)
  return bool_value_;
}

// -------------------------------------------------------------------

// Tile_Feature

inline Tile_Feature::Tile_Feature() {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
}

inline Tile_Feature::Tile_Feature(const protobuf::message& srcMsg) {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
  for (protobuf::message msg(srcMsg); msg.next(); ) {
    if (msg.tag == kIdFieldNumber) {
      id_ = msg.read_uint64();
      _has_bits_[0] |= 0x00000001u;
    }
    else if (msg.tag == kTagsFieldNumber) {
      protobuf::message packedMsg(msg.read_message());
      tags_.clear();
      tags_.reserve(packedMsg.length());
      while (packedMsg.valid()) {
        tags_.emplace_back(packedMsg.read_uint32());
      }
      _has_bits_[0] |= 0x00000002u;
    }
    else if (msg.tag == kTypeFieldNumber) {
      type_ = msg.read_int32();
      _has_bits_[0] |= 0x00000004u;
    }
    else if (msg.tag == kGeometryFieldNumber) {
      protobuf::message packedMsg(msg.read_message());
      geometry_.clear();
      geometry_.reserve(packedMsg.length());
      while (packedMsg.valid()) {
        geometry_.emplace_back(packedMsg.read_uint32());
      }
      _has_bits_[0] |= 0x00000008u;
    }
    else msg.skip();
  }
}

// optional uint64 id = 1 [default = 0];
inline bool Tile_Feature::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}

inline std::uint64_t Tile_Feature::id() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Feature.id)
  return id_;
}

// repeated uint32 tags = 2 [packed = true];
inline int Tile_Feature::tags_size() const {
  return static_cast<int>(tags_.size());
}

inline std::uint32_t Tile_Feature::tags(int index) const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Feature.tags)
  return tags_[index];
}

inline const std::vector< std::uint32_t >& Tile_Feature::tags() const {
  // @@protoc_insertion_point(field_list:vector_tile.Tile.Feature.tags)
  return tags_;
}

// optional .vector_tile.Tile.GeomType type = 3 [default = UNKNOWN];
inline bool Tile_Feature::has_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}

inline ::vector_tile::Tile_GeomType Tile_Feature::type() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Feature.type)
  return static_cast< ::vector_tile::Tile_GeomType >(type_);
}

// repeated uint32 geometry = 4 [packed = true];
inline int Tile_Feature::geometry_size() const {
  return static_cast<int>(geometry_.size());
}

inline std::uint32_t Tile_Feature::geometry(int index) const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Feature.geometry)
  return geometry_[index];
}

inline const std::vector< std::uint32_t >& Tile_Feature::geometry() const {
  // @@protoc_insertion_point(field_list:vector_tile.Tile.Feature.geometry)
  return geometry_;
}

// -------------------------------------------------------------------

// Tile_Layer

inline Tile_Layer::Tile_Layer() {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
}

inline Tile_Layer::Tile_Layer(const protobuf::message& srcMsg) {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
  std::size_t feature_count = 0;
  std::size_t key_count = 0;
  std::size_t value_count = 0;
  for (protobuf::message msg(srcMsg); msg.next(); ) {
    if (msg.tag == kVersionFieldNumber) {
      msg.read_uint32();
    }
    else if (msg.tag == kNameFieldNumber) {
      msg.read_string();
    }
    else if (msg.tag == kFeaturesFieldNumber) {
      msg.read_message();
      feature_count++;
    }
    else if (msg.tag == kKeysFieldNumber) {
      msg.read_string();
      key_count++;
    }
    else if (msg.tag == kValuesFieldNumber) {
      msg.read_message();
      value_count++;
    }
    else if (msg.tag == kExtentFieldNumber) {
      msg.read_uint32();
    }
    else msg.skip();
  }
  features_.reserve(feature_count);
  keys_.reserve(key_count);
  values_.reserve(value_count);
  for (protobuf::message msg(srcMsg); msg.next(); ) {
    if (msg.tag == kVersionFieldNumber) {
      version_ = msg.read_uint32();
      _has_bits_[0] |= 0x00000001u;
    }
    else if (msg.tag == kNameFieldNumber) {
      name_ = msg.read_string();
      _has_bits_[0] |= 0x00000002u;
    }
    else if (msg.tag == kFeaturesFieldNumber) {
      features_.emplace_back(msg.read_message());
      _has_bits_[0] |= 0x00000004u;
    }
    else if (msg.tag == kKeysFieldNumber) {
      keys_.emplace_back(msg.read_string());
      _has_bits_[0] |= 0x00000008u;
    }
    else if (msg.tag == kValuesFieldNumber) {
      values_.emplace_back(msg.read_message());
      _has_bits_[0] |= 0x00000010u;
    }
    else if (msg.tag == kExtentFieldNumber) {
      extent_ = msg.read_uint32();
      _has_bits_[0] |= 0x00000020u;
    }
    else msg.skip();
  }
}

// required uint32 version = 15 [default = 1];
inline bool Tile_Layer::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}

inline std::uint32_t Tile_Layer::version() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Layer.version)
  return version_;
}

// required string name = 1;
inline bool Tile_Layer::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}

inline const ::std::string& Tile_Layer::name() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Layer.name)
  return name_;
}

// repeated .vector_tile.Tile.Feature features = 2;
inline int Tile_Layer::features_size() const {
  return static_cast<int>(features_.size());
}

inline const ::vector_tile::Tile_Feature& Tile_Layer::features(int index) const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Layer.features)
  return features_[index];
}
inline const std::vector< ::vector_tile::Tile_Feature >& Tile_Layer::features() const {
  // @@protoc_insertion_point(field_list:vector_tile.Tile.Layer.features)
  return features_;
}

// repeated string keys = 3;
inline int Tile_Layer::keys_size() const {
  return static_cast<int>(keys_.size());
}

inline const ::std::string& Tile_Layer::keys(int index) const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Layer.keys)
  return keys_[index];
}
inline const std::vector< ::std::string>& Tile_Layer::keys() const {
  // @@protoc_insertion_point(field_list:vector_tile.Tile.Layer.keys)
  return keys_;
}

// repeated .vector_tile.Tile.Value values = 4;
inline int Tile_Layer::values_size() const {
  return static_cast<int>(values_.size());
}

inline const ::vector_tile::Tile_Value& Tile_Layer::values(int index) const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Layer.values)
  return values_[index];
}
inline const std::vector< ::vector_tile::Tile_Value >& Tile_Layer::values() const {
  // @@protoc_insertion_point(field_list:vector_tile.Tile.Layer.values)
  return values_;
}

// optional uint32 extent = 5 [default = 4096];
inline bool Tile_Layer::has_extent() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}

inline std::uint32_t Tile_Layer::extent() const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.Layer.extent)
  return extent_;
}

// -------------------------------------------------------------------

// Tile

inline Tile::Tile() {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
}

inline Tile::Tile(const protobuf::message& srcMsg) {
  std::fill(_has_bits_, _has_bits_ + sizeof(_has_bits_) / sizeof(std::uint32_t), 0);
  std::size_t layer_count = 0;
  for (protobuf::message msg(srcMsg); msg.next(); ) {
    if (msg.tag == kLayersFieldNumber) {
      msg.read_message();
      layer_count++;
    }
    else msg.skip();
  }
  layers_.reserve(layer_count);
  for (protobuf::message msg(srcMsg); msg.next(); ) {
    if (msg.tag == kLayersFieldNumber) {
      layers_.emplace_back(msg.read_message());
      _has_bits_[0] |= 0x00000001u;
    }
    else msg.skip();
  }
}

// repeated .vector_tile.Tile.Layer layers = 3;
inline int Tile::layers_size() const {
  return static_cast<int>(layers_.size());
}

inline const ::vector_tile::Tile_Layer& Tile::layers(int index) const {
  // @@protoc_insertion_point(field_get:vector_tile.Tile.layers)
  return layers_[index];
}
inline const std::vector< ::vector_tile::Tile_Layer >& Tile::layers() const {
  // @@protoc_insertion_point(field_list:vector_tile.Tile.layers)
  return layers_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace vector_tile

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_MBVTPackage_2eproto__INCLUDED
