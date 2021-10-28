#include "FSM.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <random>

FSM::FSM() {
    std::ifstream file;
    file = std::ifstream(data_path("gameConsole.json"));
	if (file.is_open()) {
		file >> gameConsole;
	}
	else {
		throw std::runtime_error("Unable to open json file");
		abort();
	}

    file = std::ifstream(data_path("actionFigure.json"));
	if (file.is_open()) {
		file >> actionFigure;
	}
	else {
		throw std::runtime_error("Unable to open json file");
		abort();
	}

    file = std::ifstream(data_path("card.json"));
	if (file.is_open()) {
		file >> card;
	}
	else {
		throw std::runtime_error("Unable to open json file");
		abort();
	}

    file = std::ifstream(data_path("emotion.json"));
	if (file.is_open()) {
		file >> emotion;
	}
	else {
		throw std::runtime_error("Unable to open json file");
		abort();
	}
}

FSM::~FSM() {}

/**
 * transfer state based on action
 * 
 * @param action action -1 - 5
 */
void FSM::transferState(int action){
    // std::cout << "current state: " << cur_state << std::endl;
    // std::cout << "player action: " << action << std::endl;
    switch(cur_state) {
        case 0: {
            if (action == 0)
                cur_state = DFA[cur_state]["start"];
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }
        case 2: case 3: case 4: {
            if (action == 2)
                cur_state = DFA[cur_state]["detail"];
            else if (action == 3)
                cur_state = DFA[cur_state]["emotion"];
            else if (action == 4)
                cur_state = DFA[cur_state]["offer"];
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }
        case 5: case 6: case 7: {
            if (action == 3) {
                cur_state = DFA[cur_state]["back"];
            }
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }   
        case 8: {
            if (action == 3) {
                if (item == 0)
                    cur_state = DFA[cur_state]["gc"];
                else if (item == 1)
                    cur_state = DFA[cur_state]["af"];
                else if (item == 2)
                    cur_state = DFA[cur_state]["card"];
            }
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }
        case 9: {
            double price = bid_price;
            if (action == 1)
                price *= 1.0;
            else if (action == 2)
                price *= 0.9;
            else if (action == 3)
                price *= 0.8;
            else if (action == 4)
                price *= 0.7;
            else if (action == 5)
                price *= 0.6;
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }

            offer_price = (int)price;

            if ((int)price >= expect_price)
                cur_state = DFA[cur_state]["over"];
            else
                cur_state = DFA[cur_state]["below"];
            break;
        }
        case 10: {
            if (action == 2) {
                cur_state = DFA[cur_state]["accept"];
                offer_price = expect_price;
            }
            else if (action == 3)
                cur_state = DFA[cur_state]["reject"];
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }
        case 11: case 15: {
            if (action == 0)
                cur_state = DFA[cur_state]["next"];
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }
        case 13: case 14: {
            if (action == 0)
                cur_state = DFA[cur_state]["resume"];
            else {
                throw std::runtime_error("invalid operation in state " 
                                            + std::to_string(cur_state));
		        abort();
            }
            break;
        }
        default : {
            throw std::runtime_error("unhandled state: " 
                                            + std::to_string(cur_state));
		    abort();
        }
    }
}

/**
 * execute on current state
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::executeState(std::vector<TextBlock>& texts){
    hideTextblocks(texts);
    switch(cur_state) {
        case 0: {
            this->init(texts);
            break;
        }
        case 1: {
            this->clearAndRand(texts);
            break;
        }
        case 2: {
            this->gcDesc(texts);
            break;
        }
        case 3: {
            this->afDesc(texts);
            break;
        }
        case 4: {
            this->cardDesc(texts);
            break;
        }
        case 5: {
            this->gcDetail(texts);
            break;
        }
        case 6: {
            this->afDetail(texts);
            break;
        }
        case 7: {
            this->cardDetail(texts);
            break;
        }   
        case 8: {
            this->emotionText(texts);
            break;
        }
        case 9: {
            this->offerPrice(texts);
            break;
        }
        case 10: {
            this->secondBid(texts);
            break;
        }
        case 11: {
            this->deal(texts);
            break;
        }
        case 12: {
            this->summary(texts);
            break;
        }
        case 13: {
            this->win(texts);
            break;
        }
        case 14: {
            this->lose(texts);
            break;
        }
        case 15: {
            this->reject(texts);
            break;
        }
        default : {
            throw std::runtime_error("unhandled execution state: " 
                                            + std::to_string(cur_state));
		    abort();
        }
    }
}

/**
 * action for state 0
 * inital state
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::init(std::vector<TextBlock>& texts){
    total_gain = 0;
    current_customer = 1;

    texts[0].text = "Start";
    texts[0].visible = true;
}

/**
 * action for state 1
 * clear game info and random new game info
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::clearAndRand(std::vector<TextBlock>& texts){
	item = rand() % 3;
    item_level = rand() % 3;
	int add_on_price = rand() % (item_level * 20 + 10);

    // true price depends on item_level
	true_price = (item_level * 2 + 2) * (30 + add_on_price);

    // expect price is the 60 - 120% of true price
	expect_price = (60 + rand() % 60) * true_price / 100;
    
    // bid price is the 120 - 160% of expect price
	bid_price = (120 + rand() % 40) * expect_price / 100;

    std::cout << "#########################" << std::endl;
    std::cout << "item: " << item << std::endl;
    std::cout << "item_level: " << item_level << std::endl;
    std::cout << "true_price: " << true_price << std::endl;
    std::cout << "expect_price: " << expect_price << std::endl;
    std::cout << "bid_price: " << bid_price << std::endl;
    std::cout << "#########################" << std::endl;

    // internal state transfer
    if (item == 0)
        cur_state = DFA[cur_state]["gc"];
    else if (item == 1)
        cur_state = DFA[cur_state]["af"];
    else if (item == 2)
        cur_state = DFA[cur_state]["card"];
    executeState(texts);
}

/**
 * action for state 2
 * display game console description and first bid price
 * display action for user to choose
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::gcDesc(std::vector<TextBlock>& texts){
    int step = (max_price - min_price) / 3;
    std::string descText = "";
    if (bid_price < step) 
        descText = gameConsole["description"]["low"];
    else if (bid_price >= step * 2)
        descText = gameConsole["description"]["high"];
    else
        descText = gameConsole["description"]["medium"];
    descText += " " + std::to_string(bid_price);

    texts[1].text = descText;

    texts[3].text = "Current Profit: " + 
        std::to_string(total_gain) + " / " + std::to_string(target_gain);

    texts[9].text = "Current Customer: " + std::to_string(current_customer);

    // use the middle three text blocks
    // show action
    texts[5].text = "View Item Detail";
    texts[6].text = "View Emotion";
    texts[7].text = "Offer Price";

    texts[1].visible = true;
    texts[3].visible = true;
    texts[5].visible = true;
    texts[6].visible = true;
    texts[7].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 3
 * display action figure description and first bid price
 * display action for user to choose
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::afDesc(std::vector<TextBlock>& texts){
    int step = (max_price - min_price) / 3;
    std::string descText = "";
    if (bid_price < step) 
        descText = actionFigure["description"]["low"];
    else if (bid_price >= step * 2)
        descText = actionFigure["description"]["high"];
    else
        descText = actionFigure["description"]["medium"];
    descText += " " + std::to_string(bid_price);

    texts[1].text = descText;

    texts[3].text = "Current Profit: " + 
        std::to_string(total_gain) + " / " + std::to_string(target_gain);
    
    texts[9].text = "Current Customer: " + std::to_string(current_customer);

    // use the middle three text blocks
    // show action
    texts[5].text = "View Item Detail";
    texts[6].text = "View Emotion";
    texts[7].text = "Offer Price";

    texts[1].visible = true;
    texts[3].visible = true;
    texts[5].visible = true;
    texts[6].visible = true;
    texts[7].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 4
 * display card description and first bid price
 * display action for user to choose
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::cardDesc(std::vector<TextBlock>& texts){
    int step = (max_price - min_price) / 3;
    std::string descText = "";
    if (bid_price < step) 
        descText = card["description"]["low"];
    else if (bid_price >= step * 2)
        descText = card["description"]["high"];
    else
        descText = card["description"]["medium"];
    descText += " " + std::to_string(bid_price);

    texts[1].text = descText;

    texts[3].text = "Current Profit: " + 
        std::to_string(total_gain) + " / " + std::to_string(target_gain);

    texts[9].text = "Current Customer: " + std::to_string(current_customer);

    // use the middle three text blocks
    // show action
    texts[5].text = "View Item Detail";
    texts[6].text = "View Emotion";
    texts[7].text = "Offer Price";


    texts[1].visible = true;
    texts[3].visible = true;
    texts[5].visible = true;
    texts[6].visible = true;
    texts[7].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 5
 * display game console detail
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::gcDetail(std::vector<TextBlock>& texts){
    std::string descText = "";
    if (item_level == 0) 
        descText = gameConsole["detail"]["low"];
    else if (item_level == 2)
        descText = gameConsole["detail"]["high"];
    else
        descText = gameConsole["detail"]["medium"];
    texts[1].text = descText;
    
    texts[6].text = "Back";

    texts[1].visible = true;
    texts[6].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 6
 * display action figure detail
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::afDetail(std::vector<TextBlock>& texts){
    std::string descText = "";
    if (item_level == 0) 
        descText = actionFigure["detail"]["low"];
    else if (item_level == 2)
        descText = actionFigure["detail"]["high"];
    else
        descText = actionFigure["detail"]["medium"];
    texts[1].text = descText;
    
    texts[6].text = "Back";

    texts[1].visible = true;
    texts[6].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 7
 * display card detail
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::cardDetail(std::vector<TextBlock>& texts){
    std::string descText = "";
    if (item_level == 0) 
        descText = card["detail"]["low"];
    else if (item_level == 2)
        descText = card["detail"]["high"];
    else
        descText = card["detail"]["medium"];
    texts[1].text = descText;
    
    texts[6].text = "Back";

    texts[1].visible = true;
    texts[6].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 8
 * display emotion
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::emotionText(std::vector<TextBlock>& texts){
    double factor = (double)bid_price / expect_price;
    std::cout << "factor: " << factor << std::endl;

    std::string descText = "";
    if (factor > 1.4) {
        int idx = rand() % emotion["emotion"]["high"].size();
        descText = emotion["emotion"]["high"][idx];
    }
    else {
        int idx = rand() % emotion["emotion"]["low"].size();
        descText = emotion["emotion"]["low"][idx];
    }

    texts[1].text = descText;
    
    texts[6].text = "Back";

    texts[1].visible = true;
    texts[6].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 9
 * offer price
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::offerPrice(std::vector<TextBlock>& texts){
    texts[1].text = "Choose your offer price";
    texts[4].text = "$" + std::to_string(bid_price);
    texts[5].text = "$" + std::to_string(bid_price * 0.9);
    texts[6].text = "$" + std::to_string(bid_price * 0.8);
    texts[7].text = "$" + std::to_string(bid_price * 0.7);
    texts[8].text = "$" + std::to_string(bid_price * 0.6);

    texts[1].visible = true;
    texts[4].visible = true;
    texts[5].visible = true;
    texts[6].visible = true;
    texts[7].visible = true;
    texts[8].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 10
 * display NPC's second Bid
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::secondBid(std::vector<TextBlock>& texts){
    texts[1].text = "The customer does not agree with the price you offer ";
    texts[1].text += ", and offer a new price: $" + std::to_string(expect_price);
    texts[1].text += ". Do you accept or not?";

    texts[5].text = "Accept";
    texts[6].text = "Reject";

    texts[1].visible = true;
    texts[5].visible = true;
    texts[6].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 11
 * display deal message
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::deal(std::vector<TextBlock>& texts){
    int gain = true_price - offer_price;
    total_gain += gain;

    texts[0].text = "Next";
    texts[1].text = "The deal has been made! Congrats on getting a new ";

    if (item == 0) texts[1].text += "game console.";
    if (item == 1) texts[1].text += "action figure.";
    if (item == 2) texts[1].text += "card.";

    texts[1].text += "From the current item you obtained, ";
    
    if (gain > 0)
        texts[1].text += "you received $" + std::to_string(gain);
    else if (gain < 0)
        texts[1].text += "you lost $" + std::to_string(gain);
    else
        texts[1].text += "you neither lose nor gain.";

    texts[3].text = "Current Profit: " + 
        std::to_string(total_gain) + " / " + std::to_string(target_gain);

    texts[0].visible = true;
    texts[1].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 15
 * reject offer state
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::reject(std::vector<TextBlock>& texts){
    texts[0].text = "Next";
    texts[1].text = "You reject the offer from customer. The customer left.";

    texts[0].visible = true;
    texts[1].visible = true;

    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 12
 * display summary message
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::summary(std::vector<TextBlock>& texts){
    current_customer++;

    if (total_gain >= target_gain)
        cur_state = DFA[cur_state]["win"];
    else if (total_gain <= -target_gain || current_customer > total_customer)
        cur_state = DFA[cur_state]["lose"];
    else
        cur_state = DFA[cur_state]["next"];
    executeState(texts);
}

/**
 * action for state 13
 * display win message
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::win(std::vector<TextBlock>& texts){
    texts[0].text = "Play Again";
    texts[1].text = "Congradulation! You win!";

    texts[0].visible = true;
    texts[1].visible = true;
    texts[3].visible = true;
    texts[9].visible = true;
}

/**
 * action for state 14
 * display lose message
 * 
 * @param texts  text boxs will be changed based on state
 */
void FSM::lose(std::vector<TextBlock>& texts){
    texts[0].text = "Play Again";
    texts[1].text = "Unfortunately! You lost!";

    texts[0].visible = true;
    texts[1].visible = true;
    texts[3].visible = true;
    texts[9].visible = true;
}

/**
* Hide all textblocks
*
* @param texts  text boxs will be changed based on state
*/
void FSM::hideTextblocks(std::vector<TextBlock>& texts) {
    for (auto& block : texts) {
        block.visible = false;
    }
}