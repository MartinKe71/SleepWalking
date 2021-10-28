#include <vector>
#include <deque>
#include <string>
#include "FontSource.hpp"
#include "data_path.hpp"
#include "json.hpp"

#include <glm/glm.hpp>

// for convenience
using json = nlohmann::json;
struct TextBlock
{
    bool visible = true;
    bool interactable = true;
    glm::vec2 anchor;
    glm::vec2 dims;
    glm::vec2 margins;
    glm::u8vec4 font_color;
    glm::u8vec4 box_color;
    std::string text;
    std::shared_ptr<FontSource> source;
    ShapeTextureProgram::BoxDrawable box;
    glm::vec4 box_size;

    TextBlock() = default;
    TextBlock(std::string font_name_,
        bool visible_,
        bool interactable_,
        unsigned int size,
        glm::vec2 anchor_,
        glm::vec2 dims_,
        glm::vec2 margins_,
        glm::u8vec4 font_color_,
        glm::u8vec4 box_color_,
        std::string text_) :
        visible(visible_),
        interactable(interactable_),
        anchor(anchor_),
        dims(dims_),
        margins(margins_),
        font_color(font_color_),
        box_color(box_color_),
        text(text_)
    {
        source = std::make_shared<FontSource>(data_path(font_name_), size);
        assert(source != nullptr);
        box_size = glm::vec4(
            anchor.x - margins.x,
            anchor.y + margins.y,
            anchor.x + dims.x + margins.x,
            anchor.y - dims.y - margins.y
        );
        shape_texture_program->SetBox(box, box_size, box_color);
    }
};

class FSM {
public:
    int cur_state = 0;

    int true_price = 0;
    int expect_price = 0;
    int bid_price = 0;

    int total_gain = 0;
    int current_customer = 1;

    // -1 : not define
    // 0 : game console
    // 1 : action figure
    // 2 : card
    int item = -1;

    // 0 : poor condition
    // 1 : used condition
    // 2 : perfect condition
    int item_level = -1;
    int offer_price = 0;

    int min_price = 60;
    int max_price = 600;

    int target_gain = 300;
    int total_customer = 20;

    json gameConsole;
    json actionFigure;
    json card;
    json emotion;

	// DFA
	// {state_id : {action : next state}}
	std::vector<std::unordered_map<std::string, int>> DFA = {
        {{"start", 1}},                                 // state 0: init state
        {{"gc", 2}, {"af", 3}, {"card", 4}},            // state 1: clear and rand
        {{"detail", 5}, {"emotion", 8}, {"offer", 9}},  // state 2: game console description + action
        {{"detail", 6}, {"emotion", 8}, {"offer", 9}},  // state 3: action figure description + action
        {{"detail", 7}, {"emotion", 8}, {"offer", 9}},  // state 4: card description + action
        {{"back", 2}},                                  // state 5: game console detail
        {{"back", 3}},                                  // state 6: action figure detail
        {{"back", 4}},                                  // state 7: card detail
        {{"gc", 2}, {"af", 3}, {"card", 4}},            // state 8: view emotion
        {{"over", 11}, {"below", 10}},                  // state 9: offer price
        {{"accept", 11}, {"reject", 15}},               // state 10: NPC second Bid
        {{"next", 12}},                                 // state 11: Deal
        {{"next", 1}, {"win", 13}, {"lose", 14}},       // state 12: Summary
        {{"resume", 0}},                                // state 13: Win
        {{"resume", 0}},                                // state 14: Lose
        {{"next", 12}}                                  // state 15: Reject offer
	};
    
    FSM();
	virtual ~FSM();

    /**
     * transfer state based on action
     * 
     * @param action action 0 - 5
     */
    void transferState(int action);

    /**
     * execute on current state
     * 
     * @param texts  text boxs will be changed based on state
     */
    void executeState(std::vector<TextBlock>& texts);

    /**
     * action for state 0
     * inital state
     * 
     * @param texts  text boxs will be changed based on state
     */
    void init(std::vector<TextBlock>& texts);

    /**
     * action for state 1
     * clear game info and random new game info
     * 
     * @param texts  text boxs will be changed based on state
     */
    void clearAndRand(std::vector<TextBlock>& texts);

    /**
     * action for state 2
     * display game console description and first bid price
     * display action for user to choose
     * 
     * @param texts  text boxs will be changed based on state
     */
    void gcDesc(std::vector<TextBlock>& texts);

    /**
     * action for state 3
     * display action figure description and first bid price
     * display action for user to choose
     * 
     * @param texts  text boxs will be changed based on state
     */
    void afDesc(std::vector<TextBlock>& texts);

    /**
     * action for state 4
     * display card description and first bid price
     * display action for user to choose
     * 
     * @param texts  text boxs will be changed based on state
     */
    void cardDesc(std::vector<TextBlock>& texts);

    /**
     * action for state 5
     * display game console detail
     * 
     * @param texts  text boxs will be changed based on state
     */
    void gcDetail(std::vector<TextBlock>& texts);

    /**
     * action for state 6
     * display action figure detail
     * 
     * @param texts  text boxs will be changed based on state
     */
    void afDetail(std::vector<TextBlock>& texts);

    /**
     * action for state 7
     * display card detail
     * 
     * @param texts  text boxs will be changed based on state
     */
    void cardDetail(std::vector<TextBlock>& texts);

    /**
     * action for state 8
     * display emotion
     * 
     * @param texts  text boxs will be changed based on state
     */
    void emotionText(std::vector<TextBlock>& texts);

    /**
     * action for state 9
     * offer price
     * 
     * @param texts  text boxs will be changed based on state
     */
    void offerPrice(std::vector<TextBlock>& texts);

    /**
     * action for state 10
     * display NPC's second Bid
     * 
     * @param texts  text boxs will be changed based on state
     */
    void secondBid(std::vector<TextBlock>& texts);

    /**
     * action for state 11
     * display deal message
     * 
     * @param texts  text boxs will be changed based on state
     */
    void deal(std::vector<TextBlock>& texts);

    /**
     * action for state 12
     * display summary message
     * 
     * @param texts  text boxs will be changed based on state
     */
    void summary(std::vector<TextBlock>& texts);

    /**
     * action for state 13
     * display win message
     * 
     * @param texts  text boxs will be changed based on state
     */
    void win(std::vector<TextBlock>& texts);

    /**
     * action for state 14
     * display lose message
     * 
     * @param texts  text boxs will be changed based on state
     */
    void lose(std::vector<TextBlock>& texts);

    /**
     * action for state 15
     * reject offer state
     * 
     * @param texts  text boxs will be changed based on state
     */
    void reject(std::vector<TextBlock>& texts);


    /**
     * Hide all textblocks
     * 
     * @param texts  text boxs will be changed based on state
    */
    void hideTextblocks(std::vector<TextBlock>& texts);
};
