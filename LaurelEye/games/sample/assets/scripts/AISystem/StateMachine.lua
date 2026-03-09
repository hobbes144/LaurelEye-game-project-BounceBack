--State machine core system 
--By: Salam Sibai
--GAM551

local StateMachine = {}
StateMachine.__index = StateMachine
local State = require("State")

function StateMachine.new()
    local self = setmetatable({}, StateMachine)

    self.states = {}
    self.current_state = nil
    self.pending_transition  = nil
    return self
end

function StateMachine:addState(state)
    self.states[state.name] = state
    state.StateMachine = self
end

function StateMachine:setInitialState(state_name)
    self.current_state = self.states[state_name]
    if self.current_state and self.current_state.onEnter then
        self.current_state:onEnter()
    end
end

--function StateMachine:forceTransition(transition_name) --forces transition to a specific transition that exists in the state machine
--    local transition = self.current_state:getTransitionByName(transition_name)
    
--    if transition then
--        self.pending_transition = transition
--    else
--        print("Error: Transition '" .. transition_name .. "' does not exist from state '" .. self.current_state.name .. "'") --change to whatever error is given in lua
--    end
--end

function StateMachine:forceTransition(state_name)
    local dest = self.states[state_name]
    if dest then
        self.pending_transition =
        {
            name = "__forced__",
            destination = dest,
            priority = -math.huge,
            condition = function() return true end,
        }
    end
end


function StateMachine:update(dt)
    if not self.current_state then return end

    if self.pending_transition then
        self:_executeTransition(self.pending_transition)
        self.pending_transition = nil
        return
    end

    if self.current_state.update then
        self.current_state.update(dt)
    end

    if self.pending_transition then
        self:_executeTransition(self.pending_transition)
        self.pending_transition = nil
        return
    end

    local transition = self.current_state:getNextTransition()
    if transition then
        self:_executeTransition(transition)
    end
end

function StateMachine:_executeTransition(transition)
    if self.current_state and self.current_state.onExit then
        self.current_state.onExit()
    end

    self.current_state = transition.destination

    if self.current_state and self.current_state.onEnter then
        self.current_state.onEnter()
    end
end

return StateMachine
