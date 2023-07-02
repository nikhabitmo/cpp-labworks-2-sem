#pragma once

#include <array>
#include <conio.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace MP3 {

static const size_t kCounterAllFrames = 73;
static const size_t kFrameNameSize = 4;
static const size_t kHeaderSize = 10;

static std::array<std::string, kCounterAllFrames> AllFrames = {"COMM", "COMR", "ENCR", "EQU2", "ETCO", "GRID", "LINK",
                                                               "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVA2",
                                                               "SEEK",
                                                               "SYLT", "TALB", "TBPM", "TCMP", "TCOM", "TCON", "TCOP",
                                                               "TDEN", "TDLY",
                                                               "TDOR",
                                                               "TDRC", "TDRL", "TDTG", "TENC", "TEXT", "TIPL", "TIT1",
                                                               "TIT2", "TIT3",
                                                               "TKEY",
                                                               "TLAN", "TLEN", "TMCL", "TMOO", "TOAL", "TOFN", "TOLY",
                                                               "TOPE", "TOWN",
                                                               "TPE1",
                                                               "TPE2", "TPE3", "TPE4", "TPOS", "TPRO", "TPUB", "TRCK",
                                                               "TRSN", "TRSO",
                                                               "TSOA", "TSOP",
                                                               "TSOT", "TSRC", "TSSE", "TSST", "TXXX", "UFID", "USER",
                                                               "USLT", "WCOM",
                                                               "WCOP", "WOAF",
                                                               "WOAR", "WOAS", "WORS", "WPAY", "WPUB", "WXXX"};
static std::map<unsigned char, std::string> content_types = {
    {0x00, "other"},
    {0x01, "lyrics"},
    {0x02, "text transcription"},
    {0x03, "movement/part name_"},
    {0x04, "events"},
    {0x05, "chord"},
    {0x06, "trivia/pop up information"},
    {0x07, "URLs to webpages"},
    {0x08, "URLs to images"}
};

static std::map<unsigned char, std::string> channel_type = {
    {0x00, "Other"},
    {0x01, "Master volume"},
    {0x02, "Front right"},
    {0x03, "Front left"},
    {0x04, "Back right"},
    {0x05, "Back left"},
    {0x06, "Front centre"},
    {0x07, "Back centre"},
    {0x08, "Subwoofer"}
};

class Frame {
 public:
  Frame(const char* name, size_t size, const char* data)
      : name_(new char[kFrameNameSize]), data_(new char[size + 1]) {
    strcpy(name_, name);
    size_ = size;
    strcpy(data_, data);
  }

  virtual const char* parseData() = 0;

  virtual ~Frame() {
    delete[] data_;
    delete[] name_;
  }

  char* getData() const {
    return data_;
  }

  const char* getName() const {
    return name_;
  }

  size_t getSize() const {
    return size_;
  }

 private:
  size_t size_;
  char* name_;
  char* data_;
};

class FileID : public Frame {
 public:
  FileID(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    char* data = new char[getSize()];
    std::string str = std::string(getData());
    for (int i = 0; i < str.length(); i += 2) {
      std::string byte = str.substr(i, 2);
      char chr = (char) strtol(byte.c_str(), nullptr, 16);
      data[i / 2] = chr;
    }
    return data;
  }
};

class TextFrames : public Frame {
 public:
  TextFrames(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    char* data = getData();
    data[0] = ' ';
    return data;
  }
};

class UserTextFrames : public Frame {
 public:
  UserTextFrames(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str;
    str += " Description: ";
    int index = 0;
    for (int i = 1; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }
    str += " Data: ";
    for (int i = index; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class URLFrames : public Frame {
 public:
  URLFrames(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    return getData();
  }
};

class UserURLFrames : public Frame {
 public:
  UserURLFrames(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str;
    str += " Descrription: ";
    int index = 0;
    for (int i = 1; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }
    str += " URL: ";
    for (int i = index; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class CommentFrames : public Frame {
 public:
  CommentFrames(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Language: ";
    for (int i = 1; i < kFrameNameSize; ++i) {
      str += getData()[i];
    }
    str += " Text: ";
    for (int i = kFrameNameSize; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class UnsynchronisedLyrics : public Frame {
 public:
  UnsynchronisedLyrics(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Language: ";
    for (int i = 1; i < kFrameNameSize; ++i) {
      str += getData()[i];
    }
    str += " Lyrics: ";
    for (int i = kFrameNameSize; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class PlayCounter : public Frame {
 public:
  PlayCounter(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    size_t cnt = (getData()[0] << 24) | (getData()[1] << 16) | (getData()[2] << 8) | getData()[3];
    std::string str = "Counting plays: " + std::to_string(cnt);
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Popularimeter : public Frame {
 public:
  Popularimeter(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Email to user: ";
    int index = 0;
    for (int i = 0; i < getSize(); ++i) {
      if (getData()[i] != 0) {
        str += getData()[i];
      } else {
        index = i + 1;
        break;
      }
    }
    str += " Rating: ";
    int rate = getData()[index] + 1;
    str += std::to_string(rate);
    str += " Counter: ";
    int cnt = (getData()[getSize() - kFrameNameSize] << 24) | (getData()[getSize() - 3] << 16) |
        (getData()[getSize() - 2] << 8) | getData()[getSize() - 1];
    str += std::to_string(cnt);
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class SynchronisedLyrics : public Frame {
 public:
  SynchronisedLyrics(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Language: ";
    for (auto i = 1; i < kFrameNameSize; ++i) {
      str += getData()[i];
    }
    if (getData()[5] == 0x01) {
      str += " Counting MPEG frames ";
    } else {
      str += " Counting ms ";
    }
    str += " Content type: " + content_types[getData()[6]];

    for (int i = 7; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class VolumeAdjustment : public Frame {
 public:
  VolumeAdjustment(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "ID info: ";
    int index = 0;
    for (int i = 0; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }

    str += " Type of channel: " + channel_type[getData()[index]];
    index++;
    str += " Volume level: " + std::to_string((getData()[index] << 8) | getData()[index + 1]) + "dB";
    index += 3;
    int peak_vol = getData()[index];
    str += " Top of the volume level: " + std::to_string(peak_vol);
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Equalisation : public Frame {
 public:
  Equalisation(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Interoplation method: ";
    if (getData()[0] == 0x00) {
      str += " No interpolation ";
    } else {
      str += " Linear interpolation ";
    }
    str += " ID : ";
    int index = 1;
    for (int i = 1; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }

    str += " Equalizer frequency: " + std::to_string((getData()[index] << 8) | getData()[index + 1]) + "Hz";
    index += 2;
    str += " Controlling volume: " + std::to_string((getData()[index] << 8) | getData()[index + 1]) + "Hz";
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class LinkedInfo : public Frame {
 public:
  LinkedInfo(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Id Frame: ";
    for (int i = 0; i < kFrameNameSize; ++i) {
      str += getData()[i];
    }
    int index = kFrameNameSize;
    str += " URL: ";
    for (int i = index; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }
    str += " Extra data: ";
    for (int i = index; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Position : public Frame {
 public:
  Position(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Frame position ";
    if (getData()[0] == 0x01) {
      str += " Counting MPEG frames: ";
    } else {
      str += " Counting ms: ";
    }
    int index = getData()[1];
    str += " Position: " + std::to_string(index / 60) + " minutes " + std::to_string(index % 60) + " seconds";
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class TermsOfUse : public Frame {
 public:
  TermsOfUse(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Language: ";
    for (auto i = 1; i < kFrameNameSize; ++i) {
      str += getData()[i];
    }
    str += " Rules using: ";
    for (auto i = kFrameNameSize; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Registration : public Frame {
 public:
  Registration(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Owner ID: ";
    int index = 0;
    for (int i = index; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }
    str += " Data encryption: ";
    std::string decode = std::string(getData());
    for (int i = 0; i < decode.length(); i += 2) {
      std::string byte = decode.substr(i, 2);
      char chr = (char) strtol(byte.c_str(), nullptr, 16);
      str += chr;
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Seek : public Frame {
 public:
  Seek(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    size_t shift = (getData()[0] << 24) | (getData()[1] << 16) | (getData()[2] << 8) | getData()[3];
    std::string str = "Min shift to the next tag: " + std::to_string(shift) + " bytes";
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class BufferSize : public Frame {
 public:
  BufferSize(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    size_t bf_size = (getData()[0] << 24) | (getData()[1] << 16) | (getData()[2] << 8) | getData()[3];
    std::string str = "Recomended size buffer: " + std::to_string(bf_size);
    if (getData()[4] & 0x01) {
      str += " Extra information consists in the tag ";
    } else {
      str += " Extra information doesn't consist in the tag ";
    }
    size_t shift = (getData()[5] << 24) | (getData()[6] << 16) | (getData()[7] << 8) | getData()[8];
    str += " Min shift to the next tag: " + std::to_string(shift) + " bytes";
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Owner : public Frame {
 public:
  Owner(const char* name, size_t size, const char* data) : Frame(name, size, data) {}
  const char* parseData() override {
    std::string str = "Cost: ";
    int index = 1;
    for (int i = index; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }
    str += " Date of buying: ";
    for (int i = index; i < getSize(); ++i) {
      if (getData()[i] == 0) {
        index = i + 1;
        break;
      } else {
        str += getData()[i];
      }
    }
    str += " Seller: ";
    for (int i = index; i < getSize(); ++i) {
      str += getData()[i];
    }
    char* data = new char[str.size() + 1];
    strcpy(data, str.c_str());
    return data;
  }
};

class Factory {
 public:
  Frame* build(const char* type, const char* data, size_t size) {
    if (strcmp(type, "TXXX") == 0) {
      return new UserTextFrames(type, size, data);
    } else if (strcmp(type, "WXXX") == 0) {
      return new UserURLFrames(type, size, data);
    } else if (strcmp(type, "USLT") == 0) {
      return new UnsynchronisedLyrics(type, size, data);
    } else if (strcmp(type, "UFID") == 0) {
      return new FileID(type, size, data);
    } else if (strcmp(type, "PCNT") == 0) {
      return new PlayCounter(type, size, data);
    } else if (strcmp(type, "POPM") == 0) {
      return new Popularimeter(type, size, data);
    } else if (strcmp(type, "SYLT") == 0) {
      return new SynchronisedLyrics(type, size, data);
    } else if (strcmp(type, "RVA2") == 0) {
      return new VolumeAdjustment(type, size, data);
    } else if (strcmp(type, "EQU2") == 0) {
      return new Equalisation(type, size, data);
    } else if (strcmp(type, "LINK") == 0) {
      return new LinkedInfo(type, size, data);
    } else if (strcmp(type, "POSS") == 0) {
      return new Position(type, size, data);
    } else if (strcmp(type, "ENCR") == 0 || strcmp(type, "GRID") == 0 || strcmp(type, "PRIV") == 0) {
      return new Registration(type, size, data);
    } else if (strcmp(type, "SEEK") == 0) {
      return new Seek(type, size, data);
    } else if (strcmp(type, "RBUF") == 0) {
      return new BufferSize(type, size, data);
    } else if (strcmp(type, "OWNE") == 0) {
      return new Owner(type, size, data);
    } else if (type[0] == 'T') {
      return new TextFrames(type, size, data);
    } else if (type[0] == 'W') {
      return new URLFrames(type, size, data);
    } else if (type[0] == 'C') {
      return new CommentFrames(type, size, data);
    }
    return new TextFrames(type, size, data);
  }
};

class ID3V2_4Parser {
 public:
  void parse(const std::filesystem::path& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      std::cerr << "could not open the file\n";
      exit(216);
    }
    char buff[kHeaderSize];
    file.read(buff, sizeof(buff));
    const char* sz = buff + 6;
    unsigned size = ((sz[0] << 21) | (sz[1] << 14) | (sz[2] << 7) | sz[3]) + kHeaderSize;
    Factory factory;
    while (file.tellg() < size) {
      char frame_info[kHeaderSize];
      file.read(frame_info, sizeof(frame_info));
      const char* frame_sz = frame_info + kFrameNameSize;
      auto frame_size = ((frame_sz[0] << 21) | (frame_sz[1] << 14) | (frame_sz[2] << 7) | frame_sz[3]);
      if (std::find(AllFrames.begin(), AllFrames.end(), frame_info) == AllFrames.end()) {
        file.seekg(frame_size, std::ios::cur);
        continue;
      }
      char data[frame_size + 1];
      file.read(data, frame_size);
      data[frame_size] = '\0';
      Frame* current_frame = factory.build(frame_info, data, frame_size);
      frames_.push_back(current_frame);
    }
  }

  const std::vector<Frame*>& getFrames() const {
    return frames_;
  }

  friend std::ostream& operator<<(std::ostream& ostream, const ID3V2_4Parser& parser) {
    for (const auto& frame : parser.getFrames()) {
      ostream << frame->getName() << ":\t " << frame->parseData() << '\n';
    }
    return ostream;
  }
 private:
  std::vector<Frame*> frames_;
};

};