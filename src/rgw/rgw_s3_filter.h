// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab ft=cpp

#pragma once

#include "rgw_tools.h"
#include <boost/container/flat_map.hpp>

class XMLObj;

using KeyNegativeFilterMap = std::map<std::string, bool>;

struct rgw_s3_key_filter {
  bool operator==(const rgw_s3_key_filter& rhs) const = default;
  std::string prefix_rule;
  std::string suffix_rule;
  std::string regex_rule;

  KeyNegativeFilterMap negative_filter_map;

  bool has_content() const;

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    ENCODE_START(2, 1, bl);
      encode(prefix_rule, bl);
      encode(suffix_rule, bl);
      encode(regex_rule, bl);
      encode(negative_filter_map, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::const_iterator& bl) {
    DECODE_START(2, bl);
      decode(prefix_rule, bl);
      decode(suffix_rule, bl);
      decode(regex_rule, bl);
      if(struct_v >= 2) {
        decode(negative_filter_map, bl);
      }
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(rgw_s3_key_filter)

using KeyValueMap = boost::container::flat_map<std::string, std::string>;
using KeyMultiValueMap = std::multimap<std::string, std::string>;

struct rgw_s3_key_value_filter {
  KeyValueMap kv;
  KeyNegativeFilterMap negative_filter_map;
  
  bool has_content() const;

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    ENCODE_START(2, 1, bl);
      encode(kv, bl);
      encode(negative_filter_map, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::const_iterator& bl) {
    DECODE_START(2, bl);
      decode(kv, bl);
      if(struct_v >= 2) {
        decode(negative_filter_map, bl);
      }
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(rgw_s3_key_value_filter)

struct rgw_s3_zone_filter {
  KeyNegativeFilterMap negative_filter_map; 
  
  bool has_content() const; 

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    ENCODE_START(1, 1, bl);
      encode(negative_filter_map, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::const_iterator& bl) {
    DECODE_START(1, bl);
      decode(negative_filter_map, bl);
    DECODE_FINISH(bl);
  }

};
WRITE_CLASS_ENCODER(rgw_s3_zone_filter)

struct rgw_s3_filter {
  rgw_s3_key_filter key_filter;
  rgw_s3_key_value_filter metadata_filter;
  rgw_s3_key_value_filter tag_filter;
  rgw_s3_zone_filter zone_filter;

  bool has_content() const;

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    ENCODE_START(3, 1, bl);
      encode(key_filter, bl);
      encode(metadata_filter, bl);
      encode(tag_filter, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::const_iterator& bl) {
    DECODE_START(3, bl);
      decode(key_filter, bl);
      decode(metadata_filter, bl);
      if (struct_v >= 2) {
        decode(tag_filter, bl);
      }
      if (struct_v >= 3) {
        decode(zone_filter, bl);
      }
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(rgw_s3_filter)

bool match(const rgw_s3_key_filter& filter, const std::string& key);

bool match(const rgw_s3_key_value_filter& filter, const KeyValueMap& kv);

bool match(const rgw_s3_key_value_filter& filter, const KeyMultiValueMap& kv);

bool match(const rgw_s3_zone_filter& filter, const std::string& zone);

bool match(const rgw_s3_filter& filter, const rgw::sal::Object* obj);
