
function resolve_dir(name, dir_cfg)

    if type(dir_cfg) == "table" then
        if dir_cfg.type == "path" then
            return dir_cfg.path
        elseif dir_cfg.type == "env" then
            paths = {}
            e = dir_cfg.env
            for i, p in pairs(dir_cfg.paths) do
                paths = table.join(paths, os.getenv(e) .. p)
            end
            return paths
        end
    else
        print("Error: " .. name .. " is not valid structure")
        exit()
    end
    return nil
end
