from cs50 import SQL
from flask import Flask, redirect, render_template, request

# Configuering data_display
data_display = Flask(__name__)

# Making sure it always reloads the templates
data_display.config["TEMPLATES_AUTO_RELOAD"] = True

# Telling it what the database is
db = SQL("sqlite:///Sensor_Signals.db")

@data_display.after_request
def after_request(response)
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@data_display.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":

        # TODO add singals into the database
