#ifndef FSPersistence_h
#define FSPersistence_h

#include <StatefulService.h>
#include <FS.h>
#include <FactoryResetService.h>

template <class T>
class FSPersistence {
 public:
  FSPersistence(JsonStateReader<T> stateReader,
                JsonStateUpdater<T> stateUpdater,
                StatefulService<T>* statefulService,
                FS* fs,
                const char* filePath,
                size_t bufferSize = DEFAULT_BUFFER_SIZE) :
      _stateReader(stateReader),
      _stateUpdater(stateUpdater),
      _statefulService(statefulService),
      _fs(fs),
      _filePath(filePath),
      _bufferSize(bufferSize),
      _updateHandlerId(0) {
    enableUpdateHandler();
  }

  void readFromFS() {
    File settingsFile = _fs->open(_filePath, "r");

    if (settingsFile) {
      DynamicJsonDocument jsonDocument = DynamicJsonDocument(_bufferSize);
      DeserializationError error = deserializeJson(jsonDocument, settingsFile);
      if (error == DeserializationError::Ok && jsonDocument.is<JsonObject>()) {
        JsonObject jsonObject = jsonDocument.as<JsonObject>();
        _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
        settingsFile.close();
        return;
      }
      settingsFile.close();
    }

    // If we reach here we have not been successful in loading the config and hard-coded defaults are now applied.
    // The settings are then written back to the file system so the defaults persist between resets. This last step is
    // required as in some cases defaults contain randomly generated values which would otherwise be modified on reset.
    applyDefaults();
    writeToFS();
  }

  bool writeToFS() {
    // create and populate a new json object
    DynamicJsonDocument jsonDocument = DynamicJsonDocument(_bufferSize);
    JsonObject jsonObject = jsonDocument.to<JsonObject>();
    _statefulService->read(jsonObject, _stateReader);

    if(!_fs->exists(FS_CONFIG_DIRECTORY)){
      Serial.println(F("Directory doesn't exists."));

    if(_fs->mkdir(FS_CONFIG_DIRECTORY)){
        Serial.println(F("Directory created."));
      }
      else{
         Serial.println(F("Can't create the directory."));
      }
    }
    else{
      Serial.println(F("Directory exists."));
    }

    // serialize it to filesystem
    File settingsFile = _fs->open(_filePath, "w");

    // failed to open file, return false
    if (!settingsFile) {
      Serial.print(F("Can't open the file: "));
      Serial.println(_filePath);
      return false;
    }
    else{
      Serial.print(_filePath);
      Serial.println(F(" was opened."));
    }

    // serialize the data to the file
    serializeJson(jsonDocument, settingsFile);
    settingsFile.close();
    return true;
  }

  void disableUpdateHandler() {
    if (_updateHandlerId) {
      _statefulService->removeUpdateHandler(_updateHandlerId);
      _updateHandlerId = 0;
    }
  }

  void enableUpdateHandler() {
    if (!_updateHandlerId) {
      _updateHandlerId = _statefulService->addUpdateHandler([&](const String& originId) { writeToFS(); });
    }
  }

 private:
  JsonStateReader<T> _stateReader;
  JsonStateUpdater<T> _stateUpdater;
  StatefulService<T>* _statefulService;
  FS* _fs;
  const char* _filePath;
  size_t _bufferSize;
  update_handler_id_t _updateHandlerId;

 protected:
  // We assume the updater supplies sensible defaults if an empty object
  // is supplied, this virtual function allows that to be changed.
  virtual void applyDefaults() {
    DynamicJsonDocument jsonDocument = DynamicJsonDocument(_bufferSize);
    JsonObject jsonObject = jsonDocument.as<JsonObject>();
    _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
  }
};

#endif  // end FSPersistence
