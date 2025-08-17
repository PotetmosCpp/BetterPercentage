#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

//TODO: add realtime new best option maybe idk
// also change best on level beat or something idk

class $modify(MyPlayLayer, PlayLayer) {
	struct Fields {
		float m_runFrom;
		float m_bestRunEnd;
		std::unordered_map<double, double> m_bestRunEnds;	// dumb name i think idk
		Mod* m_decimalPercentages = nullptr;
		float m_decimalPercentagesPercent = 0.f;
	};

	std::string getDPSavedValueKey(GJGameLevel* level) {
		if (!level) return "";
		if (level->m_levelType == GJLevelType::Editor)
			return fmt::format("percentage_{}_local_{}", practice ? "practice" : "normal", EditorIDs::getID(level));
		if (level->m_gauntletLevel)
			return fmt::format("percentage_{}_gauntlet_{}", practice ? "practice" : "normal", level->m_levelID.value());
		if (level->m_dailyID.value() == 0)
			return fmt::format("percentage_{}_{}", practice ? "practice" : "normal", level->m_levelID.value());
		return fmt::format("percentage_{}_{}_periodic_{}", practice ? "practice" : "normal", level->m_levelID.value(), level->m_dailyID.value());
	}

	void setupHasCompleted() {
		PlayLayer::setupHasCompleted();
		auto level = m_level;

		bool hasDecimalPercentages = Loader::get()->isModLoaded("raydeeux_thesillydoggo.decimalpercentages");
		if (!hasDecimalPercentages || !level) return;

		m_fields->m_decimalPercentages = Loader::get()->getLoadedMod("raydeeux_thesillydoggo.decimalpercentages");
		if (!m_fields->m_decimalPercentages) return;

		std::string savedValueKey = MyPlayLayer::getDPSavedValueKey(level);
		if (!m_fields->m_decimalPercentages->hasSavedValue(savedValueKey)) return;

		m_fields->m_decimalPercentagesPercent = m_fields->m_decimalPercentages->getSavedValue<float>(savedValueKey);
	}

	void destroyPlayer(PlayerObject* player, GameObject* object) {
		PlayLayer::destroyPlayer(player, object);

		if (object != m_anticheatSpike) {
			if (PlayLayer::getCurrentPercent() > m_fields->m_bestRunEnds[m_fields->m_runFrom]) {
				m_fields->m_bestRunEnds[m_fields->m_runFrom] = PlayLayer::getCurrentPercent();
			}
		}
	}

	void resetLevel() {
		/*checkBestRun();*/

		PlayLayer::resetLevel();

		m_fields->m_runFrom = PlayLayer::getCurrentPercent();
		/*geode::createQuickPopup(
			"Title",            // title
			"suppe",   // content
			"Nah", "Yeah",      // buttons
			[](auto, bool btn2) {
			}
		);*/
		if (!m_fields->m_decimalPercentages || !m_level) return;

		std::string savedValueKey = MyPlayLayer::getDPSavedValueKey(m_level);
		if (!m_fields->m_decimalPercentages->hasSavedValue(savedValueKey)) return;

		m_fields->m_decimalPercentagesPercent = m_fields->m_decimalPercentages->getSavedValue<float>(savedValueKey);
	}

	void updateProgressbar() {
		PlayLayer::updateProgressbar();

		auto decimals = Mod::get()->getSettingValue<int>("decimals");
		auto showPercentSymbol = Mod::get()->getSettingValue<bool>("show-percent-symbol");

		auto showRunFrom = Mod::get()->getSettingValue<bool>("show-run-from");
		auto hideRunFromFromZero = Mod::get()->getSettingValue<bool>("hide-run-from-from-zero");

		auto showBestRun = Mod::get()->getSettingValue<bool>("show-best-run");
		auto hideBestRunFromZero = Mod::get()->getSettingValue<bool>("hide-best-run-from-zero");
		auto showLevelBestFromZero = Mod::get()->getSettingValue<bool>("show-level-best-from-zero");

		auto formatNumber = [decimals](double value) {
			return fmt::format("{:.{}f}", value, decimals);
		};

		auto runFrom = (showRunFrom)
			? ((m_fields->m_runFrom == 0 && hideRunFromFromZero)
				? ""
				: formatNumber(m_fields->m_runFrom) + "-"
			)
			: "";

		auto normalPercentAsString = geode::utils::numToString(m_level->m_normalPercent);

		if (m_fields->m_decimalPercentages) {
			normalPercentAsString = geode::utils::numToString(m_fields->m_decimalPercentagesPercent);
		}

		auto bestRun = (showBestRun && !(m_fields->m_runFrom == 0 && hideBestRunFromZero))
			? ((m_fields->m_runFrom == 0 && showLevelBestFromZero)
				? " / " + normalPercentAsString
				: ((m_fields->m_bestRunEnds.count(m_fields->m_runFrom))
			 		? " / " + formatNumber(m_fields->m_bestRunEnds[m_fields->m_runFrom])	// wtf is this
					: ""
				)
	  		)
			: "";

		auto percent = (showPercentSymbol)
			? "%"
			: "";

		m_percentageLabel->setString((
			runFrom +
			formatNumber(PlayLayer::getCurrentPercent()) +
			bestRun +
			percent
		).c_str());
	}
};
