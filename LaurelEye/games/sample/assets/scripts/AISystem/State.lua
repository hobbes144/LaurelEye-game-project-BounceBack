--State structure
--By: Salam Sibai
--GAM551

--This file holds the basic architecture of states of the AI State Machine

---@class Transition
---@field name string
---@field destination State
---@field priority integer
---@field condition fun(): boolean

---@alias StateCallback fun(...): nil
---@alias StateCondition fun(): boolean

---@class State
---@field name string
---@field transitions Transition[]
---@field transitions_by_name table<string, Transition>
---@field stateMachine StateMachine|nil
---@field onEnter StateCallback|nil
---@field update StateCallback|nil
---@field onExit StateCallback|nil
local State={}              --how classes are created in lua (apparently)
State.__index = State       --method inheritance

---@param name string
---@return State
function State.new(name)
    local self = setmetatable({}, State)    --creates instance of State class

    self.name = name
    self.transitions = {}
    self.transitions_by_name = {}
    self.stateMachine = nil --will be set when state is added to a machine

    self.onEnter = nil
    self.update = nil
    self.onExit = nil
    return self
end

---@param func StateCallback
function State:setOnEnter(func)
    self.onEnter = func
end

---@param func StateCallback
function State:setUpdate(func)
    self.update = func
end

---@param func StateCallback
function State:setOnExit(func)
    self.onExit = func
end

---@param n string
---@param destination_state State
---@param _priority integer
---@param _condition StateCondition
function State:addTransition(n, destination_state, _priority, _condition)
    ---@type Transition
    local transition = {
        name = n,
        destination = destination_state,
        priority = _priority,
        condition = _condition
    }

    table.insert(self.transitions, transition)
    self.transitions_by_name[n] = transition

    table.sort(self.transitions, function(a, b)
        return a.priority < b.priority
    end)
end

---@param name string
---@return Transition|nil
function State:getNextTransitionByName(name)
    return self.transitions_by_name[name]
end

---@return Transition|nil
function State:getNextTransition()
    ---@type integer|nil
    local current_priority = nil
    ---@type Transition[]
    local valid_transitions = {}

    for _, transition in ipairs(self.transitions) do
        if current_priority and transition.priority ~= current_priority and #valid_transitions > 0 then
            return self:_pickRandom(valid_transitions)
        end

        current_priority = transition.priority

        local success, result = pcall(transition.condition)
        if success and result then
            table.insert(valid_transitions, transition)
        elseif not success then
            print("Error in transition '" .. transition.name .. "' condition: " .. tostring(result)) --replace with the error logs anish made
        end
    end

    if #valid_transitions > 0 then
        return self:_pickRandom(valid_transitions)
    end

    return nil
end

---@param transitions Transition[]
---@return Transition
function State:_pickRandom(transitions)
    if #transitions == 1 then
        return transitions[1]
    end
    return transitions[math.random(#transitions)]
end

return State

