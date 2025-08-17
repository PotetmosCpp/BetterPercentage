#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

//TODO: add realtime new best option maybe idk
// also change best on level beat or something idk

class $modify(PlayLayer) {
	struct Fields {
		float m_runFrom;
		float m_bestRunEnd;
		std::unordered_map<double, double> m_bestRunEnds;	// dumb name i think idk
	};

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
			std::ostringstream ostringstream;
			ostringstream << std::fixed << std::setprecision(decimals) << value;
			return ostringstream.str();
		};

		auto runFrom = (showRunFrom)
			? ((m_fields->m_runFrom == 0 && hideRunFromFromZero)
				? ""
				: formatNumber(m_fields->m_runFrom) + "-"
			)
			: "";

		auto bestRun = (showBestRun && !(m_fields->m_runFrom == 0 && hideBestRunFromZero))
			? ((m_fields->m_runFrom == 0 && showLevelBestFromZero)
				? "/" + std::to_string(m_level->m_normalPercent)
				: ((m_fields->m_bestRunEnds.count(m_fields->m_runFrom))
			 		? "/" + formatNumber(m_fields->m_bestRunEnds[m_fields->m_runFrom])	// wtf is this
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
