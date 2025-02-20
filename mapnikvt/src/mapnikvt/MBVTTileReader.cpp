#include "MBVTTileReader.h"
#include "Layer.h"

namespace carto::mvt {
    void MBVTTileReader::setLayerNameOverride(const std::string& name) {
        _layerNameOverride = name;
    }

    std::shared_ptr<vt::TileBackground> MBVTTileReader::createTileBackground(const vt::TileId& tileId) const {
        std::shared_ptr<const vt::BitmapPattern> backgroundBitmapPattern;
        if (!_map->getSettings().backgroundImage.empty()) {
            backgroundBitmapPattern = _symbolizerContext.getBitmapManager()->loadBitmapPattern(_map->getSettings().backgroundImage, 1.0f, 1.0f);
        }
        return std::make_shared<vt::TileBackground>(_map->getSettings().backgroundColor, backgroundBitmapPattern);
    }
    
    std::shared_ptr<FeatureDecoder::FeatureIterator> MBVTTileReader::createFeatureIterator(const std::shared_ptr<const Layer>& layer, const std::set<std::string>* fields) const {
        std::string layerName = _layerNameOverride.empty() ? layer->getName() : _layerNameOverride;
        return _featureDecoder.createLayerFeatureIterator(layerName, fields);
    }
}
