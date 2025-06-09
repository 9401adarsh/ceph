// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab ft=cpp

#pragma once

#include "rgw_tools.h"
#include <boost/container/flat_map.hpp>

class XMLObj;

struct rgw_s3_key_filter {
  bool operator==(const rgw_s3_key_filter& rhs) const = default;
  std::string prefix_rule;
  std::string suffix_rule;
  std::string regex_rule;

  bool is_prefix_negative{false};
  bool is_suffix_negative{false};
  bool is_regex_negative{false};

  bool has_content() const;

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    ENCODE_START(2, 1, bl);
      encode(prefix_rule, bl);
      encode(suffix_rule, bl);
      encode(regex_rule, bl);
      encode(is_prefix_negative, bl);
      encode(is_suffix_negative, bl);
      encode(is_regex_negative, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::const_iterator& bl) {
    DECODE_START(2, bl);
      decode(prefix_rule, bl);
      decode(suffix_rule, bl);
      decode(regex_rule, bl);
      if(struct_v >= 2) {
        decode(is_prefix_negative, bl); 
        decode(is_suffix_negative, bl); 
        decode(is_regex_negative, bl);
      }
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(rgw_s3_key_filter)

using KeyValueMap = boost::container::flat_map<std::string, std::string>;
//add this type for negative filter support - ensure v2 structs encoded as (key, (value, is_negative)) 
using KeyValueTypePairMap = boost::container::flat_map<std::string, std::pair<std::string,bool>>;
using KeyMultiValueMap = std::multimap<std::string, std::string>;

struct rgw_s3_key_value_filter {
  // key -> {value, is_negative} ; Type - IN => is_negative = false; Type - OUT => is_negative = true
  KeyValueTypePairMap kvt; 
  bool has_content() const;

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    KeyValueMap kv; 
    for(const auto& it: kvt){
      if(it.second.second) {
        kv.emplace(it.first, it.second.first);
      }
    }
    ENCODE_START(2, 1, bl);
    encode(kv, bl);
    encode(kvt, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::const_iterator& bl) {
    KeyValueMap kv;
    DECODE_START(2, bl);
      decode(kv, bl);
      if(struct_v >= 2) {
        decode(kvt, bl);
      }
      else {   
        // convert kv to kvt for correct matching based on type for v1 structs
        for (const auto& it : kv) {
          kvt.emplace(it.first, std::make_pair(it.second, true));
        }
      }
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(rgw_s3_key_value_filter)

struct rgw_s3_filter {
  rgw_s3_key_filter key_filter;
  rgw_s3_key_value_filter metadata_filter;
  rgw_s3_key_value_filter tag_filter;

  bool has_content() const;

  void dump(Formatter *f) const;
  bool decode_xml(XMLObj *obj);
  void dump_xml(Formatter *f) const;

  void encode(bufferlist& bl) const {
    ENCODE_START(2, 1, bl);
      encode(key_filter, bl);
      encode(metadata_filter, bl);
      encode(tag_filter, bl);
    ENCODE_FINISH(bl);
  }

  void decode(bufferlist::const_iterator& bl) {
    DECODE_START(2, bl);
      decode(key_filter, bl);
      decode(metadata_filter, bl);
      if (struct_v >= 2) {
        decode(tag_filter, bl);
      }
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(rgw_s3_filter)

bool match(const rgw_s3_key_filter& filter, const std::string& key);

bool match(const rgw_s3_key_value_filter& filter, const KeyValueMap& kv);

bool match(const rgw_s3_key_value_filter& filter, const KeyMultiValueMap& kv);

bool match(const rgw_s3_filter& filter, const rgw::sal::Object* obj);
